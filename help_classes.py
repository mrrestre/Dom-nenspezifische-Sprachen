from data_types import *


class SymbolTable:
    def __init__(self):
        self._symbol_table = {}

    def set_item(self, key, value):
        if isinstance(value, BaseType):
            self._symbol_table[key] = value
        else:
            raise TypeError(f"Value for key '{key}' must be an instance of BaseType, got {type(value)}")

    def get_item(self, key):
        return self._symbol_table.get(key, None)

    def __getitem__(self, key):
        return self.get_item(key)

    def __setitem__(self, key, value):
        self.set_item(key, value)

    def __str__(self):
        str_map = ''
        for variable in self._symbol_table:
            str_map += variable
            str_map += ':\t'
            str_map += self._symbol_table[variable].debug_str()
            str_map += '\n'
        str_map = str_map.rstrip("\n")
        return str_map
    
TERMINAL_NODE_NAMES = ["STRTOKEN", "NUMTOKEN", "TIMETOKEN", "BOOLTOKEN", "LIST", "EMPTYLIST", "NOW"]

def is_terminal_node(node):
    if isinstance(node, list):
        return False
    elif node["type"] in TERMINAL_NODE_NAMES:
        return True
    else:
        return False
    
class TerminalNode:
    def __init__(self, node):
        match node["type"]:
            case "STRTOKEN":
                self._value = StrType(node["value"])
            case "NUMTOKEN":
                self._value = NumType(node["value"])
            case "TIMETOKEN":
                self._value = DateType(node["value"])
            case "BOOLTOKEN":
                self._value = BoolType(node["value"])
            case "LIST":
                self._value = ListType(node["elements"])
            case "EMPTYLIST":
                self._value = ListType(None)
            case "NOW":
                self._value = DateType(datetime.strftime(datetime.now(), DATETIME_FORMAT))

    def get_value(self):
        return self._value