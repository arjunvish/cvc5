#!/usr/bin/env python
###############################################################################
# Top contributors (to current version):
#   Mathias Preiner, Everett Maus
#
# This file is part of the cvc5 project.
#
# Copyright (c) 2009-2021 by the authors listed in the file AUTHORS
# in the top-level source directory and their institutional affiliations.
# All rights reserved.  See the file COPYING in the top-level source
# directory for licensing information.
# #############################################################################
##
"""
    Generate option handling code and documentation in one pass. The generated
    files are only written to the destination file if the contents of the file
    has changed (in order to avoid global re-compilation if only single option
    files changed).

    mkoptions.py <src> <build> <dst> <toml>+

      <src>     base source directory of all toml files
      <build>   build directory to write the generated sphinx docs
      <dst>     base destination directory for all generated files
      <toml>+   one or more *_options.toml files


    This script expects the following files (within <src>):

      - <src>/main/options_template.cpp
      - <src>/options/module_template.cpp
      - <src>/options/module_template.h
      - <src>/options/options_public_template.cpp
      - <src>/options/options_template.cpp
      - <src>/options/options_template.h

    <toml>+ must be the list of all *.toml option configuration files.


    This script generates the following files:
      - <dst>/main/options.cpp
      - <dst>/options/<module>_options.cpp (for every toml file)
      - <dst>/options/<module>_options.h (for every toml file)
      - <dst>/options/options_public.cpp
      - <dst>/options/options.cpp
      - <dst>/options/options.h
"""

import os
import re
import sys
import textwrap
import toml

### Allowed attributes for module/option

MODULE_ATTR_REQ = ['id', 'name']
MODULE_ATTR_ALL = MODULE_ATTR_REQ + ['option']

OPTION_ATTR_REQ = ['category', 'type']
OPTION_ATTR_ALL = OPTION_ATTR_REQ + [
    'name', 'short', 'long', 'alias', 'default', 'alternate', 'mode',
    'handler', 'predicates', 'includes', 'minimum', 'maximum', 'help',
    'help_mode'
]

CATEGORY_VALUES = ['common', 'expert', 'regular', 'undocumented']

################################################################################
################################################################################
# utility functions


def wrap_line(s, indent, **kwargs):
    """Wrap and indent text and forward all other kwargs to textwrap.wrap()."""
    return ('\n' + ' ' * indent).join(
        textwrap.wrap(s, width=80 - indent, **kwargs))


def concat_format(s, objs):
    """Helper method to render a string for a list of object"""
    return '\n'.join([s.format(**o.__dict__) for o in objs])


def format_include(include):
    """Generate the #include directive for a given header name."""
    if '<' in include:
        return '#include {}'.format(include)
    return '#include "{}"'.format(include)


def is_numeric_cpp_type(ctype):
    """Check if given type is a numeric type (double, int64_t or uint64_t)."""
    return ctype in ['int64_t', 'uint64_t', 'double']


def die(msg):
    """Exit with the given error message."""
    sys.exit('[error] {}'.format(msg))


def all_options(modules, sorted=False):
    """Helper to iterate all options from all modules."""
    if sorted:
        options = []
        for m in modules:
            options = options + [(m, o) for o in m.options]
        options.sort(key=lambda t: t[1])
        yield from options
    else:
        for module in modules:
            if not module.options:
                continue
            for option in module.options:
                yield module, option


def write_file(directory, name, content):
    """Write content to `directory/name`. If the file exists, only overwrite it
    when the content would actually change."""
    fname = os.path.join(directory, name)
    try:
        if os.path.isfile(fname):
            with open(fname, 'r') as file:
                if content == file.read():
                    return
        with open(fname, 'w') as file:
            file.write(content)
    except IOError:
        die("Could not write to '{}'".format(fname))


def read_tpl(directory, name):
    """Read a (custom) template file from `directory/name`. Expects placeholders
    of the form `${varname}$` and turns them into `{varname}` while all other
    curly braces are replaced by double curly braces. Thus, the result is
    suitable for `.format()` with kwargs being used."""
    fname = os.path.join(directory, name)
    try:
        with open(fname, 'r') as file:
            res = file.read()
            res = res.replace('{', '{{').replace('}', '}}')
            return res.replace('${', '').replace('}$', '')
    except IOError:
        die("Could not find '{}'. Aborting.".format(fname))


################################################################################
################################################################################
# classes to represent modules and options


class Module(object):
    """Represents one options module from one <module>_options.toml file."""
    def __init__(self, d, filename):
        self.__dict__ = {k: d.get(k, None) for k in MODULE_ATTR_ALL}
        self.options = []
        self.id = self.id.lower()
        self.id_cap = self.id.upper()
        self.filename = os.path.splitext(os.path.split(filename)[-1])[0]
        self.header = os.path.join('options', '{}.h'.format(self.filename))


class Option(object):
    """Represents on option."""
    def __init__(self, d):
        self.__dict__ = dict((k, None) for k in OPTION_ATTR_ALL)
        self.includes = []
        self.predicates = []
        for (attr, val) in d.items():
            assert attr in self.__dict__
            if attr == 'alternate' or val:
                self.__dict__[attr] = val
        if self.type == 'bool' and self.alternate is None:
            self.alternate = True
        self.long_name = None
        self.long_opt = None
        if self.long:
            r = self.long.split('=', 1)
            self.long_name = r[0]
            if len(r) > 1:
                self.long_opt = r[1]
        self.names = set()
        if self.long_name:
            self.names.add(self.long_name)
        if self.alias:
            self.names.update(self.alias)

    def __lt__(self, other):
        if self.long_name and other.long_name:
            return self.long_name < other.long_name
        if self.long_name: return True
        return False

    def __str__(self):
        return self.long_name if self.long_name else self.name


################################################################################
################################################################################
# code generation functions

################################################################################
# for options/options.h


def generate_holder_fwd_decls(modules):
    """Render forward declaration of holder structs"""
    return concat_format('  struct Holder{id_cap};', modules)


def generate_holder_mem_decls(modules):
    """Render declarations of holder members of the Option class"""
    return concat_format(
        '    std::unique_ptr<options::Holder{id_cap}> d_{id};', modules)


def generate_holder_ref_decls(modules):
    """Render reference declarations for holder members of the Option class"""
    return concat_format('  options::Holder{id_cap}& {id};', modules)


################################################################################
# for options/options.cpp


def generate_module_headers(modules):
    """Render includes for module headers"""
    return concat_format('#include "{header}"', modules)


def generate_holder_mem_inits(modules):
    """Render initializations of holder members of the Option class"""
    return concat_format(
        '        d_{id}(std::make_unique<options::Holder{id_cap}>()),',
        modules)


def generate_holder_ref_inits(modules):
    """Render initializations of holder references of the Option class"""
    return concat_format('        {id}(*d_{id}),', modules)


def generate_holder_mem_copy(modules):
    """Render copy operation of holder members of the Option class"""
    return concat_format('      *d_{id} = *options.d_{id};', modules)


################################################################################
# for options/options_public.cpp


def generate_public_includes(modules):
    """Generates the list of includes for options_public.cpp."""
    headers = set()
    for _, option in all_options(modules):
        headers.update([format_include(x) for x in option.includes])
    return '\n'.join(headers)


def generate_getnames_impl(modules):
    """Generates the implementation for options::getNames()."""
    names = set()
    for _, option in all_options(modules):
        names.update(option.names)
    res = ', '.join(map(lambda s: '"' + s + '"', sorted(names)))
    return wrap_line(res, 4, break_on_hyphens=False)


def generate_get_impl(modules):
    """Generates the implementation for options::get()."""
    res = []
    for module, option in all_options(modules, True):
        if not option.name or not option.long:
            continue
        cond = ' || '.join(['name == "{}"'.format(x) for x in option.names])
        ret = None
        if option.type == 'bool':
            ret = 'return options.{}.{} ? "true" : "false";'.format(
                module.id, option.name)
        elif option.type == 'std::string':
            ret = 'return options.{}.{};'.format(module.id, option.name)
        elif is_numeric_cpp_type(option.type):
            ret = 'return std::to_string(options.{}.{});'.format(
                module.id, option.name)
        else:
            ret = '{{ std::stringstream s; s << options.{}.{}; return s.str(); }}'.format(
                module.id, option.name)
        res.append('if ({}) {}'.format(cond, ret))
    return '\n  '.join(res)


def _set_handlers(option):
    """Render handler call for options::set()."""
    optname = option.long_name if option.long else ""
    if option.handler:
        if option.type == 'void':
            return 'opts.handler().{}("{}", name)'.format(
                option.handler, optname)
        else:
            return 'opts.handler().{}("{}", name, optionarg)'.format(
                option.handler, optname)
    elif option.mode:
        return 'stringTo{}(optionarg)'.format(option.type)
    return 'handlers::handleOption<{}>("{}", name, optionarg)'.format(
        option.type, optname)


def _set_predicates(option):
    """Render predicate calls for options::set()."""
    if option.type == 'void':
        return []
    optname = option.long_name if option.long else ""
    assert option.type != 'void'
    res = []
    if option.minimum:
        res.append(
            'opts.handler().checkMinimum("{}", name, value, static_cast<{}>({}));'
            .format(optname, option.type, option.minimum))
    if option.maximum:
        res.append(
            'opts.handler().checkMaximum("{}", name, value, static_cast<{}>({}));'
            .format(optname, option.type, option.maximum))
    res += [
        'opts.handler().{}("{}", name, value);'.format(x, optname)
        for x in option.predicates
    ]
    return res


TPL_SET = '''    opts.{module}.{name} = {handler};
    opts.{module}.{name}WasSetByUser = true;'''
TPL_SET_PRED = '''    auto value = {handler};
    {predicates}
    opts.{module}.{name} = value;
    opts.{module}.{name}WasSetByUser = true;'''


def generate_set_impl(modules):
    """Generates the implementation for options::set()."""
    res = []
    for module, option in all_options(modules, True):
        if not option.long:
            continue
        cond = ' || '.join(['name == "{}"'.format(x) for x in option.names])
        predicates = _set_predicates(option)
        if res:
            res.append('  }} else if ({}) {{'.format(cond))
        else:
            res.append('if ({}) {{'.format(cond))
        if option.name and not (option.handler and option.mode):
            if predicates:
                res.append(
                    TPL_SET_PRED.format(module=module.id,
                                        name=option.name,
                                        handler=_set_handlers(option),
                                        predicates='\n    '.join(predicates)))
            else:
                res.append(
                    TPL_SET.format(module=module.id,
                                   name=option.name,
                                   handler=_set_handlers(option)))
        elif option.handler:
            h = '  opts.handler().{handler}("{smtname}", name'
            if option.type not in ['bool', 'void']:
                h += ', optionarg'
            h += ');'
            res.append(
                h.format(handler=option.handler, smtname=option.long_name))
    return '\n'.join(res)


def generate_getinfo_impl(modules):
    """Generates the implementation for options::getInfo()."""
    res = []
    for module, option in all_options(modules, True):
        if not option.long:
            continue
        constr = None
        fmt = {
            'condition': ' || '.join(['name == "{}"'.format(x) for x in option.names]),
            'name': option.long_name,
            'alias': '',
            'type': option.type,
            'value': 'opts.{}.{}'.format(module.id, option.name),
            'setbyuser': 'opts.{}.{}WasSetByUser'.format(module.id, option.name),
            'default': option.default if option.default else '{}()'.format(option.type),
            'minimum': option.minimum if option.minimum else '{}',
            'maximum': option.maximum if option.maximum else '{}',
        }
        if option.alias:
            fmt['alias'] = ', '.join(map(lambda s: '"{}"'.format(s), option.alias))
        if not option.name:
            fmt['setbyuser'] = 'false'
            constr = 'OptionInfo::VoidInfo{{}}'
        elif option.type in ['bool', 'std::string']:
            constr = 'OptionInfo::ValueInfo<{type}>{{{default}, {value}}}'
        elif option.type == 'double' or is_numeric_cpp_type(option.type):
            constr = 'OptionInfo::NumberInfo<{type}>{{{default}, {value}, {minimum}, {maximum}}}'
        elif option.mode:
            fmt['modes'] = ', '.join(['"{}"'.format(s) for s in sorted(option.mode.keys())])
            constr = 'OptionInfo::ModeInfo{{"{default}", {value}, {{ {modes} }}}}'
        else:
            constr = 'OptionInfo::VoidInfo{{}}'
        line = 'if ({condition}) return OptionInfo{{"{name}", {{{alias}}}, {setbyuser}, ' + constr + '}};'
        res.append(line.format(**fmt))
    return '\n  '.join(res)


################################################################################
# for options/<module>.h


def generate_module_includes(module):
    includes = set()
    for option in module.options:
        if option.name is None:
            continue
        includes.update([format_include(x) for x in option.includes])
    return '\n'.join(sorted(includes))


TPL_MODE_DECL = '''enum class {type}
{{
  {values}
}};
static constexpr size_t {type}__numValues = {nvalues};
std::ostream& operator<<(std::ostream& os, {type} mode);
{type} stringTo{type}(const std::string& optarg);
'''


def generate_module_mode_decl(module):
    """Generates the declarations of mode enums and utility functions."""
    res = []
    for option in module.options:
        if option.name is None or not option.mode:
            continue
        res.append(
            TPL_MODE_DECL.format(type=option.type,
                                 values=wrap_line(
                                     ', '.join(option.mode.keys()), 2),
                                 nvalues=len(option.mode)))
    return '\n'.join(res)


def generate_module_holder_decl(module):
    res = []
    for option in module.options:
        if option.name is None:
            continue
        if option.default:
            default = option.default
            if option.mode and option.type not in default:
                default = '{}::{}'.format(option.type, default)
            res.append('{} {} = {};'.format(option.type, option.name, default))
        else:
            res.append('{} {};'.format(option.type, option.name))
        res.append('bool {}WasSetByUser = false;'.format(option.name))
    return '\n  '.join(res)


def generate_module_wrapper_functions(module):
    res = []
    for option in module.options:
        if option.name is None:
            continue
        res.append(
            'inline {type} {name}() {{ return Options::current().{module}.{name}; }}'
            .format(module=module.id, name=option.name, type=option.type))
    return '\n'.join(res)


def generate_module_option_names(module):
    relevant = [
        o for o in module.options
        if not (o.name is None or o.long_name is None)
    ]
    return concat_format(
        'static constexpr const char* {name}__name = "{long_name}";', relevant)


def generate_module_setdefaults_decl(module):
    res = []
    for option in module.options:
        if option.name is None:
            continue
        funcname = option.name[0].capitalize() + option.name[1:]
        res.append('void setDefault{}(Options& opts, {} value);'.format(
            funcname, option.type))
    return '\n'.join(res)


################################################################################
# for options/<module>.cpp

TPL_MODE_STREAM_OPERATOR = '''std::ostream& operator<<(std::ostream& os, {type} mode)
{{
  switch(mode)
  {{
    {cases}
    default: Unreachable();
  }}
  return os;
}}'''

TPL_MODE_TO_STRING = '''{type} stringTo{type}(const std::string& optarg)
{{
  {cases}
  else if (optarg == "help")
  {{
    std::cerr << {help};
    std::exit(1);
  }}
  throw OptionException(std::string("unknown option for --{long}: `") +
                        optarg + "'.  Try --{long}=help.");
}}'''


def _module_mode_help(option):
    """Format help message for mode options."""
    assert option.help_mode
    assert option.mode

    text = ['R"FOOBAR(']
    text.append('  ' + wrap_line(option.help_mode, 2, break_on_hyphens=False))
    text.append('Available {}s for --{} are:'.format(option.long_opt.lower(),
                                                     option.long_name))

    for value, attrib in option.mode.items():
        assert len(attrib) == 1
        attrib = attrib[0]
        if 'help' not in attrib:
            continue
        if value == option.default and attrib['name'] != "default":
            text.append('+ {} (default)'.format(attrib['name']))
        else:
            text.append('+ {}'.format(attrib['name']))
        text.append('  '
                    + wrap_line(attrib['help'], 2, break_on_hyphens=False))
    text.append(')FOOBAR"')
    return '\n'.join(text)


def generate_module_mode_impl(module):
    """Generates the declarations of mode enums and utility functions."""
    res = []
    for option in module.options:
        if option.name is None or not option.mode:
            continue
        cases = [
            'case {type}::{enum}: return os << "{type}::{enum}";'.format(
                type=option.type, enum=x) for x in option.mode.keys()
        ]
        res.append(
            TPL_MODE_STREAM_OPERATOR.format(type=option.type,
                                            cases='\n    '.join(cases)))

        # Generate str-to-enum handler
        names = set()
        cases = []
        for value, attrib in option.mode.items():
            assert len(attrib) == 1
            name = attrib[0]['name']
            if name in names:
                die("multiple modes with the name '{}' for option '{}'".format(
                    name, option.long))
            else:
                names.add(name)

            cases.append(
                'if (optarg == "{name}") return {type}::{enum};'.format(
                    name=name, type=option.type, enum=value))
        assert option.long
        assert cases
        res.append(
            TPL_MODE_TO_STRING.format(type=option.type,
                                      cases='\n  else '.join(cases),
                                      help=_module_mode_help(option),
                                      long=option.long_name))
    return '\n'.join(res)


TPL_SETDEFAULT_IMPL = '''void setDefault{capname}(Options& opts, {type} value)
{{
    if (!opts.{module}.{name}WasSetByUser) opts.{module}.{name} = value;
}}'''


def generate_module_setdefaults_impl(module):
    res = []
    for option in module.options:
        if option.name is None:
            continue
        fmt = {
            'capname': option.name[0].capitalize() + option.name[1:],
            'type': option.type,
            'module': module.id,
            'name': option.name,
        }
        res.append(TPL_SETDEFAULT_IMPL.format(**fmt))
    return '\n'.join(res)


################################################################################
# for main/options.cpp


def _add_cmdoption(option, name, opts, next_id):
    fmt = {
        'name': name,
        'arg': 'no' if option.type in ['bool', 'void'] else 'required',
        'next_id': next_id
    }
    opts.append(
        '{{ "{name}", {arg}_argument, nullptr, {next_id} }},'.format(**fmt))


def generate_parsing(modules):
    """Generates the implementation for main::parseInternal() and matching
    options definitions suitable for getopt_long(). Returns a tuple with:
    - short options description (passed as third argument to getopt_long)
    - long options description (passed as fourth argument to getopt_long)
    - handler code that turns getopt_long return value to a setOption call
    """
    short = ""
    opts = []
    code = []
    next_id = 256
    for _, option in all_options(modules, False):
        needs_impl = False
        if option.short:  # short option
            needs_impl = True
            code.append("case '{0}': // -{0}".format(option.short))
            short += option.short
            if option.type not in ['bool', 'void']:
                short += ':'
        if option.long:  # long option
            needs_impl = True
            _add_cmdoption(option, option.long_name, opts, next_id)
            code.append('case {}: // --{}'.format(next_id, option.long_name))
            next_id += 1
        if option.alias:  # long option aliases
            needs_impl = True
            for alias in option.alias:
                _add_cmdoption(option, alias, opts, next_id)
                code.append('case {}: // --{}'.format(next_id, alias))
                next_id += 1

        if needs_impl:
            # there is some way to call it, add call to solver.setOption()
            if option.type == 'bool':
                code.append('  solver.setOption("{}", "true"); break;'.format(
                    option.long_name))
            elif option.type == 'void':
                code.append('  solver.setOption("{}", ""); break;'.format(
                    option.long_name))
            else:
                code.append(
                    '  solver.setOption("{}", optionarg); break;'.format(
                        option.long_name))

        if option.alternate:
            assert option.type == 'bool'
            # bool option that wants a --no-*
            needs_impl = False
            if option.long:  # long option
                needs_impl = True
                _add_cmdoption(option, 'no-' + option.long_name, opts, next_id)
                code.append('case {}: // --no-{}'.format(
                    next_id, option.long_name))
                next_id += 1
            if option.alias:  # long option aliases
                needs_impl = True
                for alias in option.alias:
                    _add_cmdoption(option, 'no-' + alias, opts, next_id)
                    code.append('case {}: // --no-{}'.format(next_id, alias))
                    next_id += 1
            code.append('  solver.setOption("{}", "false"); break;'.format(
                option.long_name))

    return short, '\n  '.join(opts), '\n    '.join(code)


def _cli_help_format_options(option):
    """
    Format short and long options for the cmdline documentation
    (--long | --alias | -short).
    """
    opts = []
    if option.long:
        if option.long_opt:
            opts.append('--{}={}'.format(option.long_name, option.long_opt))
        else:
            opts.append('--{}'.format(option.long_name))

    if option.alias:
        if option.long_opt:
            opts.extend(
                ['--{}={}'.format(a, option.long_opt) for a in option.alias])
        else:
            opts.extend(['--{}'.format(a) for a in option.alias])

    if option.short:
        if option.long_opt:
            opts.append('-{} {}'.format(option.short, option.long_opt))
        else:
            opts.append('-{}'.format(option.short))

    return ' | '.join(opts)


def _cli_help_wrap(help_msg, opts):
    """Format cmdline documentation (--help) to be 80 chars wide."""
    width_opt = 25
    text = textwrap.wrap(help_msg, 80 - width_opt, break_on_hyphens=False)
    if len(opts) > width_opt - 3:
        lines = ['  {}'.format(opts), ' ' * width_opt + text[0]]
    else:
        lines = ['  {}{}'.format(opts.ljust(width_opt - 2), text[0])]
    lines.extend([' ' * width_opt + l for l in text[1:]])
    return lines


def generate_cli_help(modules):
    """Generate the output for --help."""
    common = []
    others = []
    for module in modules:
        if not module.options:
            continue
        others.append('')
        others.append('From the {} module:'.format(module.name))
        for option in module.options:
            if option.category == 'undocumented':
                continue
            msg = option.help
            if option.category == 'expert':
                msg += ' (EXPERTS only)'
            opts = _cli_help_format_options(option)
            if opts:
                if option.alternate:
                    msg += ' [*]'
                res = _cli_help_wrap(msg, opts)

                if option.category == 'common':
                    common.extend(res)
                else:
                    others.extend(res)
    return '\n'.join(common), '\n'.join(others)


################################################################################
# sphinx command line documentation @ docs/options_generated.rst


def _sphinx_help_add(module, option, common, others):
    """Analyze an option and add it to either common or others."""
    names = []
    if option.long:
        if option.long_opt:
            names.append('--{}={}'.format(option.long_name, option.long_opt))
        else:
            names.append('--{}'.format(option.long_name))

    if option.alias:
        if option.long_opt:
            names.extend(
                ['--{}={}'.format(a, option.long_opt) for a in option.alias])
        else:
            names.extend(['--{}'.format(a) for a in option.alias])

    if option.short:
        if option.long_opt:
            names.append('-{} {}'.format(option.short, option.long_opt))
        else:
            names.append('-{}'.format(option.short))

    modes = None
    if option.mode:
        modes = {}
        for _, data in option.mode.items():
            assert len(data) == 1
            modes[data[0]['name']] = data[0].get('help', '')

    data = {
        'name': names,
        'help': option.help,
        'expert': option.category == 'expert',
        'alternate': option.alternate,
        'help_mode': option.help_mode,
        'modes': modes,
    }

    if option.category == 'common':
        common.append(data)
    else:
        if module.name not in others:
            others[module.name] = []
        others[module.name].append(data)


def _sphinx_help_render_option(res, opt):
    """Render an option to be displayed with sphinx."""
    indent = ' ' * 4
    desc = '``{}``'
    val = indent + '{}'
    if opt['expert']:
        res.append('.. admonition:: This option is intended for Experts only!')
        res.append(indent)
        desc = indent + desc
        val = indent + val

    if opt['alternate']:
        desc += ' (also ``--no-*``)'
    res.append(desc.format(' | '.join(opt['name'])))
    res.append(val.format(opt['help']))

    if opt['modes']:
        res.append(val.format(''))
        res.append(val.format(opt['help_mode']))
        res.append(val.format(''))
        for k, v in opt['modes'].items():
            if v == '':
                continue
            res.append(val.format(':{}: {}'.format(k, v)))
    res.append(indent)


def generate_sphinx_help(modules):
    """Render the command line help for sphinx."""
    common = []
    others = {}
    for module, option in all_options(modules, False):
        if option.type == 'undocumented':
            continue
        if not option.long and not option.short:
            continue
        _sphinx_help_add(module, option, common, others)

    res = []
    res.append('Most Commonly-Used cvc5 Options')
    res.append('===============================')
    for opt in common:
        _sphinx_help_render_option(res, opt)

    res.append('')
    res.append('Additional cvc5 Options')
    res.append('=======================')
    for module in others:
        res.append('')
        res.append('{} Module'.format(module))
        res.append('-' * (len(module) + 8))
        for opt in others[module]:
            _sphinx_help_render_option(res, opt)

    return '\n'.join(res)


################################################################################
# main code generation for individual modules


def codegen_module(module, dst_dir, tpls):
    """Generate code for one option module."""
    data = {
        'id_cap': module.id_cap,
        'id': module.id,
        # module header
        'includes': generate_module_includes(module),
        'modes_decl': generate_module_mode_decl(module),
        'holder_decl': generate_module_holder_decl(module),
        'wrapper_functions': generate_module_wrapper_functions(module),
        'option_names': generate_module_option_names(module),
        'setdefaults_decl': generate_module_setdefaults_decl(module),
        # module source
        'header': module.header,
        'modes_impl': generate_module_mode_impl(module),
        'setdefaults_impl': generate_module_setdefaults_impl(module),
    }
    for tpl in tpls:
        filename = tpl['output'].replace('module', module.filename)
        write_file(dst_dir, filename, tpl['content'].format(**data))


################################################################################
# main code generation


def codegen_all_modules(modules, build_dir, dst_dir, tpls):
    """Generate code for all option modules."""
    short, cmdline_opts, parseinternal = generate_parsing(modules)
    help_common, help_others = generate_cli_help(modules)

    if os.path.isdir('{}/docs/'.format(build_dir)):
        write_file('{}/docs/'.format(build_dir), 'options_generated.rst',
                   generate_sphinx_help(modules))

    data = {
        # options/options.h
        'holder_fwd_decls': generate_holder_fwd_decls(modules),
        'holder_mem_decls': generate_holder_mem_decls(modules),
        'holder_ref_decls': generate_holder_ref_decls(modules),
        # options/options.cpp
        'headers_module': generate_module_headers(modules),
        'holder_mem_inits': generate_holder_mem_inits(modules),
        'holder_ref_inits': generate_holder_ref_inits(modules),
        'holder_mem_copy': generate_holder_mem_copy(modules),
        # options/options_public.cpp
        'options_includes': generate_public_includes(modules),
        'getnames_impl': generate_getnames_impl(modules),
        'get_impl': generate_get_impl(modules),
        'set_impl': generate_set_impl(modules),
        'getinfo_impl': generate_getinfo_impl(modules),
        # main/options.cpp
        'help_common': help_common,
        'help_others': help_others,
        'cmdoptions_long': cmdline_opts,
        'cmdoptions_short': short,
        'parseinternal_impl': parseinternal,
    }
    for tpl in tpls:
        write_file(dst_dir, tpl['output'], tpl['content'].format(**data))


################################################################################
# sanity checking


class Checker:
    """Performs a variety of sanity checks on options and option modules, and
    constructs `Module` and `Option` from dictionaries."""
    def __init__(self):
        self.__filename = None
        self.__long_cache = {}

    def perr(self, msg, *args, **kwargs):
        """Print an error and die."""
        if 'option' in kwargs:
            msg = "option '{}' {}".format(kwargs['option'], msg)
        msg = 'parse error in {}: {}'.format(self.__filename, msg)
        die(msg.format(*args, **kwargs))

    def __check_module_attribs(self, req, valid, module):
        """Check the attributes of an option module."""
        for k in req:
            if k not in module:
                self.perr("required module attribute '{}' not specified", k)
        for k in module:
            if k not in valid:
                self.perr("invalid module attribute '{}' specified", k)

    def __check_option_attribs(self, req, valid, option):
        """Check the attributes of an option."""
        if 'name' in option:
            name = option['name']
        else:
            name = option.get('long', '--')
        for k in req:
            if k not in option:
                self.perr(
                    "required option attribute '{}' not specified for '{}'", k,
                    name)
        for k in option:
            if k not in valid:
                self.perr("invalid option attribute '{}' specified for '{}'",
                          k, name)

    def __check_option_long(self, option, long):
        """Check a long argument of an option (name and uniqueness)."""
        if long.startswith('--'):
            self.perr("remove '--' prefix from '{}'", long, option=option)
        r = r'^[0-9a-zA-Z\-]+$'
        if not re.match(r, long):
            self.perr("long '{}' does not match '{}'", long, r, option=option)
        if long in self.__long_cache:
            file = self.__long_cache[long]
            self.perr("long '{}' was already defined in '{}'",
                      long,
                      file,
                      option=option)
        self.__long_cache[long] = self.__filename

    def check_module(self, module, filename):
        """Check the given module and return a `Module` object."""
        self.__filename = os.path.basename(filename)
        self.__check_module_attribs(MODULE_ATTR_REQ, MODULE_ATTR_ALL, module)
        return Module(module, filename)

    def check_option(self, option):
        """Check the option module and return an `Option` object."""
        self.__check_option_attribs(OPTION_ATTR_REQ, OPTION_ATTR_ALL, option)
        o = Option(option)
        if o.category not in CATEGORY_VALUES:
            self.perr("has invalid category '{}'", o.category, option=o)
        if o.mode and not o.help_mode:
            self.perr('defines modes but no help_mode', option=o)
        if o.mode and not o.default:
            self.perr('mode option has no default', option=o)
        if o.mode and o.default and o.default not in o.mode.keys():
            self.perr("invalid default value '{}'", o.default, option=o)
        if o.short and not o.long:
            self.perr("has short '{}' but no long", o.short, option=o)
        if o.category != 'undocumented' and not o.help:
            self.perr("of type '{}' has no help text", o.category, option=o)
        if o.alias and not o.long:
            self.perr('has aliases but no long', option=o)
        if o.alternate and o.type != 'bool':
            self.perr('is alternate but not bool', option=o)
        if o.long:
            self.__check_option_long(o, o.long_name)
            if o.alternate:
                self.__check_option_long(o, 'no-' + o.long_name)
            if o.type in ['bool', 'void'] and '=' in o.long:
                self.perr('must not have an argument description', option=o)
            if o.type not in ['bool', 'void'] and not '=' in o.long:
                self.perr("needs argument description ('{}=...')",
                          o.long,
                          option=o)
            if o.alias:
                for alias in o.alias:
                    self.__check_option_long(o, alias)
                    if o.alternate:
                        self.__check_option_long(o, 'no-' + alias)
        return o


################################################################################
# main entrypoint


def usage():
    """Print the command-line usage"""
    print('mkoptions.py <src> <build> <dst> <toml>+')
    print('')
    print('  <src>     base source directory of all toml files')
    print('  <build>   build directory to write the generated sphinx docs')
    print('  <dst>     base destination directory for all generated files')
    print('  <toml>+   one or more *_options.toml files')
    print('')


def mkoptions_main():
    if len(sys.argv) < 5:
        usage()
        die('missing arguments')

    # Load command line arguments
    _, src_dir, build_dir, dst_dir, *filenames = sys.argv

    # Check if given directories exist.
    for d in [src_dir, dst_dir]:
        if not os.path.isdir(d):
            usage()
            die("'{}' is not a directory".format(d))

    # Check if given configuration files exist.
    for file in filenames:
        if not os.path.exists(file):
            die("configuration file '{}' does not exist".format(file))

    module_tpls = [
        {'input': 'options/module_template.h'},
        {'input': 'options/module_template.cpp'},
    ]
    global_tpls = [
        {'input': 'options/options_template.h'},
        {'input': 'options/options_template.cpp'},
        {'input': 'options/options_public_template.cpp'},
        {'input': 'main/options_template.cpp'},
    ]

    # Load all template files
    for tpl in module_tpls + global_tpls:
        tpl['output'] = tpl['input'].replace('_template', '')
        tpl['content'] = read_tpl(src_dir, tpl['input'])

    # Parse and check toml files
    checker = Checker()
    modules = []
    for filename in filenames:
        data = toml.load(filename)
        module = checker.check_module(data, filename)
        if 'option' in data:
            module.options = sorted(
                [checker.check_option(a) for a in data['option']])
        modules.append(module)

    # Generate code
    for module in modules:
        codegen_module(module, dst_dir, module_tpls)
    codegen_all_modules(modules, build_dir, dst_dir, global_tpls)


if __name__ == "__main__":
    mkoptions_main()
    sys.exit(0)
