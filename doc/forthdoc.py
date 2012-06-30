# Documentation module for Forth vocabularies

import re

from docutils import nodes
from sphinx import addnodes
from sphinx.util.compat import Directive


html_extra = ['<table width="100%"><tr><td style="width:20%">',
              '</td><td>',
              '</td><td style="text-align:right">',
              '</td></tr></table>']
tex_extra = [r'\hbox to 0.3\textwidth{',
             r'}',
             r'\hfill',
             r'']

def extras(i):
    return [nodes.raw('', tex_extra[i], format='latex'),
            nodes.raw('', html_extra[i], format='html')]

def parse_worddef(env, sig, signode):
    match = re.match(r'(\S+)\s+'
                     r'(\(.*\)|)\s*'
                     r'([^"]*?)\s*'
                     r'(".*"|)\s*'
                     r'$', sig + ' ')
    if not match:
        raise ValueError

    name, stack, flags, say = match.groups()

    signode += extras(0)
    signode += addnodes.desc_name(name, name)
    signode += extras(1)
    for s in stack.split():
        if s in ['(', '--', ')'] or s.endswith(':'):
            signode += nodes.inline(s, s)
        else:
            signode += nodes.emphasis(s, s)
        signode += nodes.inline(' ', ' ')
    signode += extras(2)
    signode += nodes.inline(flags, flags)
    signode += nodes.inline(' ', ' ')
    signode += nodes.emphasis(say, say)
    signode += extras(3)
    return name
    
class Source(Directive):

    required_arguments = 1
    optional_arguments = 0
    final_argument_whitespace = True
    option_spec = {}
    has_content = False

    def run(self):
        return [nodes.paragraph(),
                nodes.emphasis(text='Source: '),
                nodes.inline(text=self.arguments[0] + '.')]
 
def setup(app):
    # *app* is a Sphinx object in sphinx/application.py

    app.add_object_type('word', 'word', 'single: %s', parse_node=parse_worddef)
    app.add_directive('source', Source)
