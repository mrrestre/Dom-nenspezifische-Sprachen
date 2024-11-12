from data_types import BaseType, ListType


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
            if(isinstance(self._symbol_table[variable], ListType)):
                for list_element in self._symbol_table[variable]:
                    str_map += str(list_element)
                    str_map += ' '
            else:
                str_map += str(self._symbol_table[variable])
            str_map += '\n'
        str_map = str_map.rstrip("\n")
        return str_map