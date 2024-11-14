from datetime import datetime
from abc import ABC, abstractmethod

DATE_FORMAT = "%Y-%m-%d"
DATETIME_FORMAT = "%Y-%m-%dT%H:%M:%S"

class BaseType(ABC):
    def __init__(self, input, timestamp=None):
        self.value = self.parse_value(input)
        self.timestamp = datetime.strptime(timestamp, DATETIME_FORMAT) if timestamp is not None else None

    @abstractmethod
    def parse_value(self, value):
        pass

    def debug_str(self):
        if isinstance(self.timestamp, datetime):
            timestamp = self.timestamp.strftime(DATETIME_FORMAT)
        else:
            timestamp = None

        return f"{str(self.value)} (Timestamp: {timestamp})"
    
    def copy(self):
        return self.__class__(self.value, self._get_timestamp_string())
    
    def _get_timestamp_string(self):
        if self.timestamp != None:
            return datetime.strftime(self.timestamp, DATETIME_FORMAT) 
        else: 
            return None

    def __str__(self):
        return str(self.value)
    
    def __add__(self, other):
        return NullType(None)
    
    def __sub__(self, other):
        return NullType(None)
            

class NullType(BaseType):
    def parse_value(self, input):
        return 'null'
        
class StrType(BaseType):
    def parse_value(self, input):
        return str(input)

class NumType(BaseType):
    def parse_value(self, input):
        return float(input)
    
    def __add__(self, other):
        if isinstance(other, NumType):
            return NumType(self.value + other.value)
        else:
            return NullType(None)
        
    def __sub__(self, other):
        if isinstance(other, NumType):
            return NumType(self.value - other.value)
        else:
            return NullType(None)

class DateType(BaseType):
    def parse_value(self, input):
        try:
            if(isinstance(input, datetime)):
                return input
            else: 
                return datetime.strptime(input, DATETIME_FORMAT)
        except ValueError:
            return datetime.strptime(input, DATE_FORMAT)

class BoolType(BaseType):
    def parse_value(self, input):
        if input.strip().lower() == "true":
            return 'true'
        else:
            return 'false'

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
                if element.get('type') == 'NULLTOKEN':
                    self.value.append(NullType(None))
        
        return self.value
    
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
            timestamp = element.timestamp.strftime(DATETIME_FORMAT) if element.timestamp != None else None
            debug_str +=  f"{str(element)} (Timestamp: {timestamp}), "
        debug_str = debug_str.rstrip(", ")
        debug_str += ']'
        return debug_str