from datetime import datetime
from abc import ABC, abstractmethod

date_format = "%Y-%m-%d"
datetime_format = "%Y-%m-%d %H:%M:%S"

class BaseType(ABC):
    def __init__(self, input, timestamp=None):
        self.value = self.parse_value(input)
        self.timestamp = datetime.strptime(timestamp, datetime_format) if timestamp is not None else None

    @abstractmethod
    def parse_value(self, value):
        pass

    def debug_str(self):
        timestamp = self.timestamp.strftime(datetime_format) if self.timestamp is not None else None
        return f"{str(self.value)} (Timestamp: {timestamp})"

    def __str__(self):
        return str(self.value)
    
    def __add__(self, other):
        if isinstance(other, BaseType):
            return self.value + other.value
        else:
            return NotImplemented

class NullType(BaseType):
    def parse_value(self, input):
        return 'null'

    def __add__(self, other):
        return 'null'
        
class StrType(BaseType):
    def parse_value(self, input):
        return str(input)

    def __add__(self, other):
        if isinstance(other, StrType):
            return StrType(self.value + other.value)
        else:
            return NullType()

class NumType(BaseType):
    def parse_value(self, input):
        return float(input)
    
    def __add__(self, other):
        if isinstance(other, NumType):
            return NumType(self.value + other.value)
        else:
            return NullType(None)

class DateType(BaseType):
    def parse_value(self, input):
        try:
            if(isinstance(input, datetime)):
                return datetime
            else: 
                return datetime.strptime(input, datetime_format)
        except ValueError:
            return datetime.strptime(input, date_format)
    
    def __add__(self, other):
        raise TypeError("Addition not supported for DateType")

class BoolType(BaseType):
    def parse_value(self, input):
        if input.strip().lower() == "true":
            return 'true'
        else:
            return 'false'
    
    def __add__(self, other):
        raise TypeError("Addition not supported for BoolType")

class ListType(BaseType):
    allowed_types = (StrType, NumType, DateType, BoolType, NullType)
    
    def parse_value(self, input):
        self.value = []
        if input:
            for element in input:
                if element.get('type') == 'NUMTOKEN':
                    self.value.append(NumType(element.get('value')))
                if element.get('type') == 'STRTOKEN':
                    self.value.append(StrType(element.get('value')))
                if element.get('type') == 'TIMETOKEN':
                    self.value.append(DateType(element.get('value')))
                if element.get('type') == 'BOOLTOKEN':
                    self.value.append(BoolType(element.get('value')))
        
        return self.value
    
    def __add__(self, other):
        raise TypeError("Addition not supported for ListType")
    
    def __getitem__(self, index):
        return self.value[index]

    def __len__(self):
        return len(self.value)

    def __str__(self):
        list_str = '['
        for element in self.value:
            list_str += f"{element.value}, "
        list_str = list_str.rstrip(", ")
        list_str += ']'
        return list_str
    
    def __setitem__(self, index, item):
        if isinstance(item, self.allowed_types):
            self.value[index] = item
        else:
            raise TypeError(f"Only instances of {self.allowed_types} are allowed")
    
    def append(self, item):
        if isinstance(item, self.allowed_types):
            self.value.append(item)
        else:
            raise TypeError(f"Only instances of {self.allowed_types} are allowed")

    def debug_str(self):
        debug_str = '['
        for element in self.value:
            timestamp = element.timestamp.strftime(datetime_format) if element.timestamp != None else None
            debug_str +=  f"{str(element)} (Timestamp: {timestamp}), "
        debug_str = debug_str.rstrip(", ")
        debug_str += ']'
        return debug_str