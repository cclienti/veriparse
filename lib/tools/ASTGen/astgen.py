#!/usr/bin/env python3

import sys
import os
import os.path
import random
import string
import copy
import re
import argparse
import yaml
import jinja2

from collections import OrderedDict
from yaml_ordered_loader import OrderedDictYAMLLoader


RESERVED_MEMBER_VARIABLES = ['children', 'node_type', 'node_category', 'filename', 'line',

                             # C++ reserved keywords
                             'alignas', 'alignof', 'and', 'and_eq', 'asm', 'auto', 'bitand', 'bitor',
                             'bool', 'break', 'case', 'catch', 'char', 'char16_t', 'char32_t', 'class',
                             'compl', 'const', 'constexpr', 'const_cast', 'continue', 'decltype', 'default',
                             'delete', 'do', 'double', 'dynamic_cast', 'else', 'enum', 'explicit', 'export',
                             'extern', 'false', 'float', 'for', 'friend', 'goto', 'if', 'inline', 'int',
                             'long', 'mutable', 'namespace', 'new', 'noexcept', 'not', 'not_eq', 'nullptr',
                             'operator', 'or', 'or_eq', 'private', 'protected', 'public', 'register',
                             'reinterpret_cast', 'return', 'short', 'signed', 'sizeof', 'static', 'static_assert',
                             'static_cast', 'struct', 'switch', 'template', 'this', 'thread_local', 'throw',
                             'true', 'try', 'typedef', 'typeid', 'typename', 'union', 'unsigned', 'using',
                             'virtual', 'void', 'volatile', 'wchar_t', 'while', 'xor', 'xor_eq']


####################
# Jinja2 Helpers
####################

def is_virtual_node(node_desc):
    """Check if a node description marked as virtual."""
    if 'virtual' in node_desc:
        virtual = node_desc['virtual']
        return virtual
    else:
        return False


def is_type_list(str_type):
    return str_type.find('list(') == 0


def prepend_list_element(var_str, l):
    return [var_str + e for e in l]


def list_concat(list1, list2):
    return list(list1) + list(list2)


def remove_namespace(var_str):
    if isinstance(var_str, str):
        tok = var_str.split("::")
        if len(tok) > 1:
            tok = tok[1:]
        return '::'.join(tok)
    else:
        return string


def remove_escaped_quote(var_str):
    if isinstance(var_str, str):
        s = str(var_str)

        if s[0] in ["\"", "\'"]:
            s = var_str[1:]

        if s[-1] in ["\"", "\'"]:
            s = s[:-1]

        return s

    else:
        return var_str


def get_type(str_type):
    """Return the type of the children.

    Examples:
    - Node -> Node::NodePtr
    - list(Node) -> Node::NodeListPtr
    - list(Source) -> Source::SourceListPtr

    """

    node_type = re.sub(r'list\((.*)\)', r'\1', str_type)
    if is_type_list(str_type) is True:
        return '{0}::ListPtr'.format(node_type)
    else:
        return '{0}::Ptr'.format(node_type)


def get_underlying_type(str_type):
    """Return the node type of the children.

    Examples:
    - Node -> Node
    - list(Node) -> Node
    - list(Source) -> Source

    """

    return re.sub(r'list\((.*)\)', r'\1', str_type)


def get_random_word(length):
    return ''.join(random.choice(string.ascii_lowercase) for i in range(length))


def get_random_property_value(property_name, properties_dict, properties_user_enum):
    property_type = properties_dict[property_name]

    if property_type in properties_user_enum:
        return "{}::{}".format(property_type,
                               random.choice(properties_user_enum[property_type]))

    elif properties_dict[property_name] == "std::string":
        return "\"" + get_random_word(10) + "\""

    elif properties_dict[property_name] == "bool":
        return random.choice(["true", "false"])

    else:
        return str(random.randint(0, 127))


def update_dict(dict1, dict2):
    """Update dict1 with a copy of dict2 values. Dict2 values are added
    after Dict1 values (ordered dict). For convenience we return
    dict1.

    """

    for key, value in dict2.items():
        dict1[key] = value

    return dict1


def merge_dict(dict1, dict2):
    """Return a copy of dict1 updated with dict2 entries."""
    dictout = copy.deepcopy(dict1)
    return update_dict(dictout, dict2)


def merge_set(set1, set2):
    """Merge the two set."""
    list1 = list(set1)
    list2 = list(set2)
    return set(list1 + list2)


def flatten_inherited(inherited):
    """Return a flattened inherited dict."""
    new_inherited = {'children': OrderedDict(),
                     'children_underlying_types': set(),
                     'properties': OrderedDict(),
                     'properties_user_enum': OrderedDict()}

    for _, base_desc in inherited.items():
        update_dict(new_inherited['children'], base_desc['children'])
        new_inherited['children_underlying_types'] = merge_set(new_inherited['children_underlying_types'],
                                                               base_desc['children_underlying_types'])
        update_dict(new_inherited['properties'], base_desc['properties'])
        update_dict(new_inherited['properties_user_enum'], base_desc['properties_user_enum'])

    return new_inherited


####################
# Helpers
####################

def save_file_if_changed(filename, content):
    """Write the content in the file using the given filename. If the file
    already exists and if the content is the same, the file is kept
    untouched.

    """

    try:
        old_content = open(filename).read()
        if content == old_content:
            sys.stderr.write('{} not modified\n'.format(filename))
            return

    except IOError:
        pass

    try:
        f = open(filename, 'w')
        f.write(content)

    except IOError:
        sys.stderr.write('could not write file: {}\n'.format(sys.exc_info()[0]))


def is_dict(obj):

    """Return true if the object is a dict or an ordered dict."""

    if isinstance(obj, dict):
        return True

    elif isinstance(obj, OrderedDict):
        return True

    else:
        return False


def get_output_filename(dirname, filename, extension):
    """Return output filename."""
    return os.path.realpath(dirname) + '/' + filename.lower() + extension


def load_description(description_filename):
    """Load the YAML description and return the data structure."""

    try:
        description = yaml.load(open(description_filename), Loader=OrderedDictYAMLLoader)
    except IOError:
        sys.stderr.write('cannot open {}\n'.format(description_filename))
        return None

    return description


def get_base_class(description, node_str):
    """Return the base class of the node."""

    if node_str in description:
        node_desc = description[node_str]
        if 'inherit' in node_desc:
            base_class = node_desc['inherit']
        else:
            base_class = "Node"
        return base_class
    else:
        sys.stderr.write('error: node {} not found in the description\n'.format(node_str))

    return None


def get_base_classes(description, node_str):
    """Return all the base classes (recursively)."""

    base_class = get_base_class(description, node_str)

    if base_class is None:
        return []
    elif base_class is "Node":
        return ["Node"]
    else:
        return [base_class] + get_base_classes(description, base_class)


####################
# Description parser
####################

def retrieve_children_info(children, c_dict, c_underlying_type_list):
    if children is not None:
        for cname, ctype in children.items():
            c_dict[cname] = ctype
            if ctype != 'Node':
                c_underlying_type_list.add(get_underlying_type(ctype))


def retrieve_properties_info(properties, p_dict, p_enum_dict, class_name=None):
    if properties is not None:
        for pname, ptype in properties.items():
            ptype = "".join(ptype.split())
            if ptype[0:5] == "enum(" and ptype[-1] == ')':
                if class_name is None:
                    class_name = ""
                else:
                    class_name += "::"
                enum_name = "{}{}Enum".format(class_name, pname.capitalize())
                enum_list = ptype[5:-1].split(',')
                p_dict[pname] = enum_name
                p_enum_dict[enum_name] = [e.upper() for e in enum_list]
            else:
                p_dict[pname] = ptype


def get_nodes_dict_from_description(description):
    nodes_dict = OrderedDict()

    for node_name, node_desc in description.items():
        if type(node_name) is not str:
            sys.stderr.write('error: Node {} is not a string\n'.format(node_name))
            return

        if is_dict(node_desc) is False:
            sys.stderr.write('error: Node {} description is not a dictionary\n'.format(node_name))
            return

        if is_virtual_node(node_desc) is True:
            continue

        if isinstance(node_name, str) and node_name not in nodes_dict:
            pass
        else:
            sys.stderr.write('bad node name or node name redeclared: "{}"\n'.format(node_name))
            return

        if 'includes' in node_desc:
            node_includes = node_desc['includes']
            if isinstance(node_includes, str):
                node_includes = [node_includes]
        else:
            node_includes = []

        node_inheritance = get_base_classes(description, node_name)
        node_inheritance_no_node = node_inheritance.copy()
        node_inheritance_no_node.remove("Node")
        inherited = OrderedDict()
        for base_name in node_inheritance_no_node:
            inherited[base_name] = OrderedDict()

        # Parse Children
        children_dict = OrderedDict()
        children_underlying_types = set()
        if 'children' in node_desc:
            retrieve_children_info(node_desc['children'],
                                   children_dict, children_underlying_types)

        # Parse Children in base classes
        for base_name in node_inheritance_no_node:
            base_children_dict = OrderedDict()
            base_children_underlying_types = set()
            inherited[base_name]['children'] = base_children_dict
            inherited[base_name]['children_underlying_types'] = base_children_underlying_types

            base_desc = description[base_name]
            if 'children' in base_desc:
                retrieve_children_info(base_desc['children'],
                                       base_children_dict, base_children_underlying_types)

        # Parse Properties
        properties_user_enum = OrderedDict()
        properties_dict = OrderedDict()
        if 'properties' in node_desc:
            retrieve_properties_info(node_desc['properties'],
                                     properties_dict, properties_user_enum)

        # Parse Properties in base classes
        for base_name in node_inheritance_no_node:
            base_properties_dict = OrderedDict()
            base_properties_user_enum = OrderedDict()
            inherited[base_name]['properties'] = base_properties_dict
            inherited[base_name]['properties_user_enum'] = base_properties_user_enum

            base_desc = description[base_name]
            if 'properties' in base_desc:
                retrieve_properties_info(base_desc['properties'],
                                         base_properties_dict, base_properties_user_enum, class_name=base_name)

        # Gather results
        nodes_dict[node_name] = {"includes": node_includes,
                                 "base_class": node_inheritance[0],
                                 "inherited": inherited,
                                 "properties": properties_dict,
                                 "properties_user_enum": properties_user_enum,
                                 "children": children_dict,
                                 "children_underlying_types": sorted(children_underlying_types)}

    return nodes_dict


####################
# Processors
####################

def process_description(description_filename, jinja_env, header_dir, impl_dir):
    """Load the YAML description and templates. Then iterates over the
    node name to generate corresponding C++ files. Header and
    implementation generation path can be different.

    """

    description = load_description(description_filename)
    if description is None:
        return

    header_template = jinja_env.get_template('node_header.hpp')
    impl_template = jinja_env.get_template('node_impl.cpp')

    # Walk through all defined class in the description
    for node_name, ndict in get_nodes_dict_from_description(description).items():
        sys.stderr.write('Processing node {}\n'.format(node_name))

        header = header_template.render(class_name=node_name,
                                        base_class=ndict["base_class"],
                                        includes=ndict["includes"],
                                        inherited=ndict["inherited"],
                                        children_dict=ndict["children"],
                                        children_underlying_types=ndict["children_underlying_types"],
                                        properties_user_enum=ndict["properties_user_enum"],
                                        properties_dict=ndict["properties"])

        impl = impl_template.render(class_name=node_name,
                                    base_class=ndict["base_class"],
                                    inherited=ndict["inherited"],
                                    children_dict=ndict["children"],
                                    properties_user_enum=ndict["properties_user_enum"],
                                    properties_dict=ndict["properties"])

        output_filename_header = get_output_filename(header_dir+'/AST', node_name, '.hpp')
        output_filename_impl = get_output_filename(impl_dir+'/AST', node_name, '.cpp')

        save_file_if_changed(output_filename_header, header)
        save_file_if_changed(output_filename_impl, impl)


def process_node_type(description_filename, jinja_env, header_dir, impl_dir):
    sys.stderr.write('Processing node_type\n')

    description = load_description(description_filename)
    if description is None:
        return

    header_template = jinja_env.get_template('node_type.hpp')
    impl_template = jinja_env.get_template('node_type.cpp')

    nodes_dict = get_nodes_dict_from_description(description)

    header = header_template.render(nodes_dict=nodes_dict)
    impl = impl_template.render(nodes_dict=nodes_dict)

    output_filename_header = get_output_filename(header_dir+'/AST', 'node_type', '.hpp')
    output_filename_impl = get_output_filename(impl_dir+'/AST', 'node_type', '.cpp')

    save_file_if_changed(output_filename_header, header)
    save_file_if_changed(output_filename_impl, impl)


def process_node_to_node_type(description_filename, jinja_env, header_dir, impl_dir):
    sys.stderr.write('Processing node_to_node_type\n')

    description = load_description(description_filename)
    if description is None:
        return

    header_template = jinja_env.get_template('node_to_node_type.hpp')

    nodes_dict = get_nodes_dict_from_description(description)

    header = header_template.render(nodes_dict=nodes_dict)

    output_filename_header = get_output_filename(header_dir+'/AST', 'node_to_node_type', '.hpp')

    save_file_if_changed(output_filename_header, header)


def process_node_category(description_filename, jinja_env, header_dir, impl_dir):
    sys.stderr.write('Processing node_category\n')

    description = load_description(description_filename)
    if description is None:
        return

    header_template = jinja_env.get_template('node_category.hpp')
    impl_template = jinja_env.get_template('node_category.cpp')

    category_list = sorted(set([ndict["category"] for ndict in get_nodes_dict_from_description(description).values()]))

    header = header_template.render(category_list=category_list)
    impl = impl_template.render(category_list=category_list)

    output_filename_header = get_output_filename(header_dir+'/AST', 'node_category', '.hpp')
    output_filename_impl = get_output_filename(impl_dir+'/AST', 'node_category', '.cpp')

    save_file_if_changed(output_filename_header, header)
    save_file_if_changed(output_filename_impl, impl)


def process_node_ostream(description_filename, jinja_env, header_dir, impl_dir):
    sys.stderr.write('Processing node_ostream\n')

    description = load_description(description_filename)
    if description is None:
        return

    header_template = jinja_env.get_template('node_ostream.hpp')
    impl_template = jinja_env.get_template('node_ostream.cpp')

    nodes_dict = get_nodes_dict_from_description(description)

    header = header_template.render(nodes_dict=nodes_dict)
    impl = impl_template.render(nodes_dict=nodes_dict)

    output_filename_header = get_output_filename(header_dir+'/AST', 'node_ostream', '.hpp')
    output_filename_impl = get_output_filename(impl_dir+'/AST', 'node_ostream', '.cpp')

    save_file_if_changed(output_filename_header, header)
    save_file_if_changed(output_filename_impl, impl)


def process_node_general_header(description_filename, jinja_env, header_dir):
    sys.stderr.write('Processing node_general_header\n')

    description = load_description(description_filename)
    if description is None:
        return

    template = jinja_env.get_template('nodes.hpp')

    header_list = get_nodes_dict_from_description(description).keys()

    result = template.render(header_list=header_list)

    output_filename = get_output_filename(header_dir+'/AST', 'nodes', '.hpp')

    save_file_if_changed(output_filename, result)


def process_generator_base(description_filename, jinja_env, header_dir):
    sys.stderr.write('Processing generator_base\n')

    description = load_description(description_filename)
    if description is None:
        return

    template = jinja_env.get_template('generator_base.hpp')

    type_list = get_nodes_dict_from_description(description).keys()

    result = template.render(node_type_list=type_list)

    output_filename = get_output_filename(header_dir+'/generators', 'generator_base', '.hpp')

    save_file_if_changed(output_filename, result)


def process_yaml_generator(description_filename, jinja_env, header_dir, impl_dir):

    sys.stderr.write('Processing YAML generator\n')

    description = load_description(description_filename)
    if description is None:
        return

    header_template = jinja_env.get_template('yaml_generator.hpp')
    impl_template = jinja_env.get_template('yaml_generator.cpp')

    nodes_dict = get_nodes_dict_from_description(description)

    header_result = header_template.render(nodes_dict=nodes_dict)
    impl_result = impl_template.render(nodes_dict=nodes_dict)

    header_output_filename = get_output_filename(header_dir+'/generators', 'yaml_generator', '.hpp')
    impl_output_filename = get_output_filename(impl_dir+'/generators', 'yaml_generator', '.cpp')

    save_file_if_changed(header_output_filename, header_result)
    save_file_if_changed(impl_output_filename, impl_result)


def process_dot_generator(description_filename, jinja_env, header_dir, impl_dir):

    sys.stderr.write('Processing Dot generator\n')

    description = load_description(description_filename)
    if description is None:
        return

    header_template = jinja_env.get_template('dot_generator.hpp')
    impl_template = jinja_env.get_template('dot_generator.cpp')

    nodes_dict = get_nodes_dict_from_description(description)

    header_result = header_template.render(nodes_dict=nodes_dict)
    impl_result = impl_template.render(nodes_dict=nodes_dict)

    header_output_filename = get_output_filename(header_dir+'/generators', 'dot_generator', '.hpp')
    impl_output_filename = get_output_filename(impl_dir+'/generators', 'dot_generator', '.cpp')

    save_file_if_changed(header_output_filename, header_result)
    save_file_if_changed(impl_output_filename, impl_result)


def process_test_yaml_generator(description_filename, jinja_env, header_dir, impl_dir):

    sys.stderr.write('Processing Test YAML generator\n')

    description = load_description(description_filename)
    if description is None:
        return

    impl_template = jinja_env.get_template('test_yaml_generator.cpp')

    nodes_dict = get_nodes_dict_from_description(description)

    impl_result = impl_template.render(nodes_dict=nodes_dict)

    impl_output_filename = get_output_filename(impl_dir+'/../test/generators', 'test_yaml_generator', '.cpp')

    save_file_if_changed(impl_output_filename, impl_result)


def process_yaml_importer(description_filename, jinja_env, header_dir, impl_dir):

    sys.stderr.write('Processing YAML importer\n')

    description = load_description(description_filename)
    if description is None:
        return

    header_template = jinja_env.get_template('yaml_importer.hpp')
    impl_template = jinja_env.get_template('yaml_importer.cpp')
    specialization_template = jinja_env.get_template('yaml_specializations.hpp')

    nodes_dict = get_nodes_dict_from_description(description)

    header_result = header_template.render(nodes_dict=nodes_dict)
    impl_result = impl_template.render(nodes_dict=nodes_dict)
    specialization_result = specialization_template.render(nodes_dict=nodes_dict)

    header_output_filename = get_output_filename(header_dir+'/importers', 'yaml_importer', '.hpp')
    impl_output_filename = get_output_filename(impl_dir+'/importers', 'yaml_importer', '.cpp')
    specialization_output_filename = get_output_filename(header_dir+'/importers', 'yaml_specializations', '.hpp')

    save_file_if_changed(header_output_filename, header_result)
    save_file_if_changed(impl_output_filename, impl_result)
    save_file_if_changed(specialization_output_filename, specialization_result)


def process_test_yaml_importer(description_filename, jinja_env, header_dir, impl_dir):

    sys.stderr.write('Processing Test YAML importer\n')

    description = load_description(description_filename)
    if description is None:
        return

    impl_template = jinja_env.get_template('test_yaml_importer.cpp')

    nodes_dict = get_nodes_dict_from_description(description)

    impl_result = impl_template.render(nodes_dict=nodes_dict)

    impl_output_filename = get_output_filename(impl_dir+'/../test/importers', 'test_yaml_importer', '.cpp')

    save_file_if_changed(impl_output_filename, impl_result)


#################
# Main
#################

def main():
    parser = argparse.ArgumentParser(description='Generate the C++ AST classes from a YAML description.')
    parser.add_argument('description', type=str, help='template path')
    parser.add_argument('-t', '--template', type=str, default='./templates',
                        help='template path')
    parser.add_argument('-i', '--header', type=str, default='.',
                        help='header files output path')
    parser.add_argument('-c', '--impl', type=str, default='.',
                        help='implem. files output path')

    args = parser.parse_args()

    jinja_env = jinja2.Environment(loader=jinja2.FileSystemLoader(os.path.normpath(args.template)))

    jinja_env.globals['random_seed'] = random.seed
    jinja_env.globals['is_type_list'] = is_type_list
    jinja_env.globals['remove_namespace'] = remove_namespace
    jinja_env.globals['prepend_list_element'] = prepend_list_element
    jinja_env.globals['list_concat'] = list_concat
    jinja_env.globals['get_type'] = get_type
    jinja_env.globals['remove_escaped_quote'] = remove_escaped_quote
    jinja_env.globals['get_underlying_type'] = get_underlying_type
    jinja_env.globals['get_random_word get'] = get_random_word
    jinja_env.globals['get_random_property_value'] = get_random_property_value
    jinja_env.globals['merge_dict'] = merge_dict
    jinja_env.globals['merge_set'] = merge_set
    jinja_env.globals['flatten_inherited'] = flatten_inherited

    process_description(args.description, jinja_env, args.header, args.impl)
    process_node_to_node_type(args.description, jinja_env, args.header, args.impl)
    process_node_type(args.description, jinja_env, args.header, args.impl)
    process_node_general_header(args.description, jinja_env, args.header)
    process_generator_base(args.description, jinja_env, args.header)
    process_node_ostream(args.description, jinja_env, args.header, args.impl)
    process_yaml_generator(args.description, jinja_env, args.header, args.impl)
    process_dot_generator(args.description, jinja_env, args.header, args.impl)
    process_test_yaml_generator(args.description, jinja_env, args.header, args.impl)
    process_yaml_importer(args.description, jinja_env, args.header, args.impl)
    process_test_yaml_importer(args.description, jinja_env, args.header, args.impl)


if __name__ == '__main__':
    main()
