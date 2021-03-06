# Documentation module for Forth vocabularies.
# Copyright 2011-2014 Markus Redeker <cep@ibp.de>
#
# Published under the GNU General Public License version 2 or any
# later version, at your choice. There is NO WARRANY, not at all. See
# the file "copying" for details.

import re
import string
import itertools
import base64

from docutils import nodes
from sphinx import addnodes
from sphinx.directives import ObjectDescription
from sphinx.domains import Index, Domain, ObjType
from sphinx.roles import XRefRole
from sphinx.util import ws_re
from sphinx.util.nodes import make_refnode
from sphinx.util.compat import Directive


rootcontext = u'~'

html_extra = ['<span>',
              '<span style="position:absolute; left:50%; margin-top:3px">',
              '</span><span style="position:absolute; right:20px">',
              '</span></span>']
tex_extra = [r'\hbox to 0.3\textwidth{',
             r'\hss}',
             r'\hfill',
             r'']

def extras(i):
    return [nodes.raw('', tex_extra[i], format='latex'),
            nodes.raw('', html_extra[i], format='html')]

def forth_sortname(context, name):
    name = name.lower()

    match = re.match('([^a-zA-Z0-9]+)([a-zA-Z0-9].*)$', name)
    if match:
        pair = match.group(2), match.group(1)
    else:
        pair = name, u''

    c = pair[0][0]
    if c in string.letters or c in string.digits:
        return (1,) + pair + (context,)
    else:
        return (0,) + pair + (context,)

def forth_heading(c):
    """Return the index header for a word beginning with *c*."""
    if c in string.letters or c in string.digits:
        return c.upper()
    else:
        return u'Symbols'

def longname(context, name):
    """Transform a word *name* with a given *context* into it normalised form.

    The normalised form of a context
    The root context, '~', specifies absolute context paths.
    'ctx~', 'foo~ bar~'   -> 'ctx~ foo~ bar~'
    'ctx~', '~ foo~ bar~' -> 'foo~ bar~'
    'ctx~', '~'           -> '~'
    '~', 'foo~ bar~'      -> 'foo~ bar~'
    """
    context = context.split()
    name = name.split()

    if name[0] == rootcontext:
        return ' '.join(name[1:])
    elif context[0] == rootcontext:
        return ' '.join(context[1:] + name)
    else:
        return ' '.join(context + name)

class ForthWord(object):

    def __init__(self, context, name, docname, stack):
        self.context = context
        self.name = name
        self.docname = docname
        self.stack = stack

        self.longname = longname(context, name)

        # A somewhat brutal method to enforce a valid HTML anchor for
        # every Forth word is to encode it in base 64.
        self.fullname = 'word-' + base64.b64encode(self.longname)

        self.sortname = forth_sortname(context, name)
        self.heading = forth_heading(self.sortname[1][0])

    def __repr__(self):
        return '%s(%r)' % (self.__class__.__name__, self.name)

    def __cmp__(self, other):
        return cmp(self.sortname, other.sortname)

class WordDirective(ObjectDescription):

    def handle_signature(self, sig, signode):
        match = re.match(r'(\S+)\s+'
                         r'(\(.*\)|)\s*'
                         r'([^"]*?)\s*'
                         r'(".*"|)\s*'
                         r'$', sig + ' ')
        if not match:
            raise ValueError

        name, stack, flags, say = match.groups()
        self.stack = stack

        # Process flags text
        flags, messages = self.state.inline_text(flags, self.lineno)

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
        signode += flags
        signode += nodes.inline(' ', ' ')
        signode += nodes.emphasis(say, say)
        signode += extras(3)
        return name

    def add_target_and_index(self, name_cls, sig, signode):
        context = self.env.domaindata['forth']['currentcontext']
        word = ForthWord(context, name_cls, self.env.docname, self.stack)

        signode['ids'].append(word.fullname)
        self.env.domaindata['forth']['words'][word.longname] = word

class CurrentcontextDirective(Directive):

    required_arguments = 1
    optional_arguments = 0
    final_argument_whitespace = True
    option_spec = {}
    has_content = False

    def run(self):
        env = self.state.document.settings.env
        env.domaindata['forth']['currentcontext'] = self.arguments[0]
        return []

class SourceDirective(Directive):

    required_arguments = 1
    optional_arguments = 0
    final_argument_whitespace = True
    option_spec = {}
    has_content = False

    def run(self):
        subnode = nodes.paragraph()
        subnode += [nodes.emphasis(text='Source: ')]
        inodes, messages = self.state.inline_text(self.arguments[0],
                                                  self.lineno)
        subnode.extend(inodes)
        return [subnode] + messages

class ForthXRefRole(XRefRole):

    def process_link(self, env, refnode, has_explicit_title, title, target):
        context = env.domaindata['forth']['currentcontext']
        return title.split()[-1], ws_re.sub(' ', longname(context, target))

class ForthIndex(Index):

    name = 'words'
    localname = 'Index of Forth Words'
    shortname = 'words'

    def generate(self, docnames=None):
        wordlist = self.domain.env.domaindata['forth']['words']
        words = sorted(wordlist.itervalues())

        content = []
        for head, g in itertools.groupby(words, lambda w: w.heading):
            entries = []
            for w in g:
                context = '' if w.context == rootcontext else w.context
                entries += [(w.name, 0, w.docname, w.fullname,
                             context, '', w.stack)]
            content += [(head, entries)]
        return (content, True)

class ForthDomain(Domain):

    name = 'forth'
    label = 'Forth'
    object_types = {        # type (usually directive) name -> ObjType instance
        'word': ObjType('word', 'word'),
        }
    directives = {                      #: directive name -> directive class
        'word': WordDirective,
        'source': SourceDirective,
        'currentcontext': CurrentcontextDirective
        }
    roles = {                           # role name -> role callable
        'word' : ForthXRefRole()
        }
    indices = [ForthIndex]              # a list of Index subclasses
    initial_data = {                    # data value for a fresh environment
        'words': {},                    # name -> ForthWord(name)
        'currentcontext': rootcontext   # Name of the active context
        }

    def resolve_xref(self, env, fromdocname, builder,
                     typ, target, node, contnode):
        try:
            word = env.domaindata['forth']['words'][target]
            return make_refnode(builder, fromdocname,
                                word.docname, word.fullname, contnode, target)
        except KeyError:
            return


def setup(app):
    # *app* is a Sphinx object in sphinx/application.py
    app.add_domain(ForthDomain)
