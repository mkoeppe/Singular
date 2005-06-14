from exceptions import *
from copy import copy
from cd import *
from omexceptions import *
from objects import *
from re import sub
class Context(object):
    #TODO: Referenzen durch scope richtig behandeln
    def __init__(self):
        self.scope=Scope()
        self.implementations={}
        self.XMLEncoder=SimpleXMLEncoder()
    def addCDImplementation(self, implementation):
        self.implementations[implementation.cd]=implementation
    def lookupImplementation(self, oms):
        try:
            return self.implementations[oms.cd][oms]
        except KeyError:
            raise NotImplementedError
    def __getitem__(self,itemname):
        return self.scope[itemname]
    def __setitem__(self,itemname, item):
        self.scope[itemname]=item
    def push(self, dict):
        """push a lexical context in form of a dictionary"""
        self.scope.push(dict)
    def pop(self, dict):
        """pop a lexical context"""
        return self.scope.pop(dict)
    def scopeFromCurrentScope(self):
        """returns a new Scope object, sharing the dictionaries,
           which will represent the current scope"""
        return self.scope.derriveScope()
    def toGeneric(self, o):
        return o.getValue()

    def evaluateSymbol(self, oms):
        try:
            impl=self.lookupImplementation(oms)
            if len(oms.attributes)>0:
                impl=copy(impl)
                impl.attributes=copy(oms.attributes)
            return impl
        except NotImplementedError:
            print "not found"
            return oms
    def evaluate(self,omobject):
        return omobject.evaluate(self)
    def evaluateInScope(self,omobject, scope):
        bak=self.scope
        self.scope=scope
        erg=self.evaluate(omobject)
        self.scope=bak
        #print "my erg is", erg
        return erg
    def package(self, val):
        if isinstance(val, OMobject):
            return val
        else:
            if isinstance(val, int):
                return OMint(val)
    def apply(self,func,args):
        return func(self,*args)
    def XMLEncodeBody(self,body):
        return self.XMLEncoder.encode(body)

class SimpleXMLEncoder(object):
    def encode(self, string):
        return sub("<","&lt;",sub("&","&amp;",string))
        
class Scope(object):
    def __init__(self):
        self.dicts=[]
    def push(self, dict):
        self.dicts.append(dict)
    def pop(self):
        return self.dicts.pop()
    def __getitem__(self,itemname):
        i=len(self.dicts)-1
        while i>=0:
            try:
                return self.dicts[i][itemname]
            except KeyError:
                pass
            i=i-1
        raise OutOfScopeError
    def  __setitem__(self, itemname, item):
        try:
            self.dicts[len(self.dicts)-1][itemname]=item
        except IndexError:
            print "scope has no layers"
    def derriveScope(self):
        erg=Scope()
        erg.dicts=copy(self.dicts)
        return erg