from myDataTypes import BaseType


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
        return str(self._symbol_table)