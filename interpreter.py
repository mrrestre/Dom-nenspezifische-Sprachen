import json
import math

from data_types import *
from help_classes import *

TERMINAL_NODE_NAMES = ["STRTOKEN", "NUMTOKEN", "TIMETOKEN", "BOOLTOKEN", "LIST", "EMPTYLIST", "NOW"]

class Interpreter:
    def __init__(self, path):
        self.debug = False
        self.symbol_table = SymbolTable()
        self.ast = {}
        self.it = None

        with open(path, 'r') as file:
            self.ast = json.load(file)

        self.statement_handlers = {
            "ASSIGN": self.handle_assign,
            "ASSIGN_TIME": self.handle_assign_time,
            "IDENTIFIER": self.handle_identifier,
            "IF": self.handle_if,
            "FOR": self.handle_for,
            "LIST_ASSIGN": self.handle_list_index_assign,
            "STATEMENTBLOCK": self.handle_statement_block,
            "TRACE": self.handle_trace,
            "TRACE_TIME": self.handle_trace_time,
            "WRITE": self.handle_write,
            "WRITE_TIME": self.handle_write_time,
        }

        self.unary_operations_member_hdl = {
            "IS_NUMBER": self.handle_is_number,
            "SIN": self.handle_sin,
            "NOT": self.handle_not,
        }

        self.unary_operations_default_hdl = {
            "COUNT": self.handle_count,
            "FIRST": self.handle_first,
            "IS_LIST": self.handle_is_list,
            "IS_NOT_LIST": self.handle_is_not_list,
        }

        self.unary_operations = self.unary_operations_member_hdl | self.unary_operations_default_hdl

        self.binary_operations = {
            "AMPERSAND": self.handle_string_concat,
            "DIVIDE": self.handle_divide,
            "MINUS": self.handle_minus,
            "LT": self.handle_less_than,
            "LTEQ": self.handle_less_equal_than,
            "GT": self.handle_greater_than,
            "GTEQ": self.handle_greater_equal_than,
            "LIST_SHORTHAND": self.handle_list_shorthand,
            "OCCUR_AFTER": self.handle_occur_after,
            "OCCUR_BEFORE": self.handle_occur_before,
            "PLUS": self.handle_plus,
            "TIMES": self.handle_times,
            "WHERE": self.handle_where,
        }

        self.ternary_operations = { 
            "IS_WITHIN": self.handle_is_within,
        }

        self.operation_handlers = self.unary_operations | self.binary_operations | self.ternary_operations

        self.allowed_operations = list(self.operation_handlers.keys()) 

    def interpret(self):
        self.eval_node(self.ast)

    def eval_node(self, node):
        if self.is_terminal_node(node):
            return TerminalNode(node).get_value()
        elif self.is_operator_node(node):
            if not self.operation_handlers.get(node["type"]):
                raise ValueError(f"Handler not found for node type: {node["type"]}")
            
            if node["type"] == "WHERE":
                self.it = self.eval_node(node["arg"][0])
                        
                    
            evaluated_params = self.evaluate_params(node)
            operation_handler = self.operation_handlers.get(node["type"])

            match len(evaluated_params):
                # Unary operation
                case 1:
                    argument = evaluated_params[0]

                    if node["type"] in self.unary_operations_member_hdl:
                        if self.is_list(argument):
                            result = ListType(None)
                            for list_member in argument:
                                result.append(operation_handler(list_member))
                            return result
                        else:
                            return operation_handler(argument)
                        
                    elif node["type"] in self.unary_operations_default_hdl:
                        return operation_handler(argument)

                # Binary operation
                case 2:
                    left = evaluated_params[0]
                    right = evaluated_params[1]

                    # Both are lists
                    if self.is_list(left) and self.is_list(right):
                        if len(left) != len(right):
                            return NullType(None)
                        
                        # Perform the operation position based: result[i] = left[i] <operation> right[i]
                        result = ListType(None)
                        for i in range(len(left)):
                            result.append(operation_handler(left[i], right[i]))
                        return result
                    # Only right is a list
                    elif not self.is_list(left) and self.is_list(right):
                        # Perform the operation on left with all elements of right: result[i] = left <operation> right[i]
                        result = ListType(None)
                        for i in range(len(right)):
                            result.append(operation_handler(left, right[i]))
                        return result
                    # Only left is a list
                    elif self.is_list(left) and not self.is_list(right):
                        # Perform the operation on left with all elements of right: result[i] = left[i] <operation> right
                        result = ListType(None)
                        for i in range(len(left)):
                            result.append(operation_handler(left[i], right))
                        return result
                    # Neither arg is a list
                    elif not self.is_list(left) and not self.is_list(right):
                        return operation_handler(left, right)
                
                # Ternary operation
                case 3:
                    left = evaluated_params[0]
                    mid = evaluated_params[1]
                    right = evaluated_params[2]

                    # Left and right are lists
                    if self.is_list(left) and self.is_list(right):
                        if len(left) != len(right):
                            return NullType(None)
                        
                        result = ListType(None)
                        for i in range(len(left)):
                            result.append(operation_handler(left[i], mid, right[i]))
                        return result
                    # Only left is list
                    elif self.is_list(left) and not self.is_list(right):
                        result = ListType(None)
                        for element in left:
                            result.append(operation_handler(element, mid, right))
                        return result
                    # Neither is a list
                    elif not self.is_list(left) and not self.is_list(right):
                        return operation_handler(left, mid, right)
                    
        else:
            if not self.statement_handlers.get(node["type"]):
                raise ValueError(f"Handler not found for node type: {node["type"]}")
            
            return self.statement_handlers.get(node["type"])(node)

    # Statement Block
    def handle_statement_block(self, node):
        for statement in node["statements"]:
            self.eval_node(statement)
            self.it = None

    # Statements
    def handle_assign(self, node):
        input = self.eval_node(node["arg"])
        if self.symbol_table.isKeyPresent(node['varname']):
            self.symbol_table[node['varname']].value = input.value
        else:
            self.symbol_table.update({node['varname']: input.copy()})
        self.debug_symbol_list()
            
    def handle_assign_time(self, node):
        timestamp = self.eval_node(node["arg"])
        self.symbol_table[node['varname']].timestamp = timestamp.value
        self.debug_symbol_list()
    
    def handle_identifier(self, node):
        if self.symbol_table.isKeyPresent(node['name']):
            dict_value = self.symbol_table[node['name']]
            if self.is_list(dict_value):
                result = ListType(None)
                for value in dict_value:
                    if isinstance(value, Identifier):
                        if self.symbol_table.isKeyPresent(value.variable_name):
                            result.append(self.symbol_table[value.variable_name])
                        else:
                            return NullType(None)
                    else:
                        result.append(value)
                return result
            else:
                if isinstance(dict_value, Identifier):
                    if self.symbol_table.isKeyPresent(dict_value.variable_name):
                        return self.symbol_table[value.variable_name]
                    else:
                        return NullType(None)
                else:
                    return dict_value

            #return self.symbol_table[node['name']]
        else:
            return 'Variable ' + node['name'] + ' in line ' + node['line'] + ' not defined in scope'

    def handle_for(self, node):
        variable_name = node['variable']['value']
        self.symbol_table.set_item(variable_name, NullType(None))
        for_range = self.eval_node(node['list'][0])
        for i in for_range:
            self.symbol_table.update({variable_name: NumType(i.value)})
            self.eval_node(node['do'])

    def handle_if(self, node):
        conditional_result = self.eval_node(node['condition'])
        if isinstance(conditional_result, BoolType) and conditional_result.bool_value() == True:
            self.eval_node(node['thenbranch'])
        elif 'elsebranch' in node:
            self.eval_node(node['elsebranch'])

    def handle_list_index_assign(self, node):
        new_value = self.evaluate_params(node)[0]
        if self.symbol_table.isKeyPresent(node['listname']):
            symbol_table_list = self.symbol_table[node['listname']]
            index = int(node['index'])
            symbol_table_list[index] = new_value
        self.debug_symbol_list()

    def handle_write(self, node):
        result = self.eval_node(node["arg"])
        print(result.value if result.value else result)
    
    def handle_write_time(self, node):
        result = self.eval_node(node["arg"])
        print(result.timestamp)

    def handle_trace(self, node):
        line = node["line"]
        result = self.eval_node(node["arg"])
        print("Line", line, "-", result)

    def handle_trace_time(self, node):
        line = node["line"]
        result = self.eval_node(node["arg"])
        print("Line", line, "-", result.timestamp)

    ## Operations
    # Unary
    def handle_not(self, arg):
        if isinstance(arg, BoolType):
            return BoolType(arg.negate())
        else:
            return NullType(None)

    def handle_sin(self, arg):
        if isinstance(arg, NumType):
            return NumType(math.sin(arg.value))
        else:
            return NullType(None)
    
    def handle_is_number(self, arg):
        if isinstance(arg, NumType):
            return BoolType('true')
        else:
            return BoolType('false')
    
    def handle_count(self, arg):
        if self.is_list(arg):
            return NumType(len(arg))
        else:
            return NullType(None)
    
    def handle_first(self, arg):
        if self.is_list(arg):
            return NumType(arg[0])
        else:
            return NullType(None)
        
    def handle_is_list(self, arg):
        if self.is_list(arg):
            return BoolType('true')
        else:
            return BoolType('false')
        
    def handle_is_not_list(self, arg):
        return BoolType(self.handle_is_list(arg).negate())

    # Binary
    def handle_less_than(self, left, right):
        return BoolType(left < right)
    
    def handle_less_equal_than(self, left, right):
        return BoolType(left <= right)
    
    def handle_greater_than(self, left, right):
        return BoolType(left > right)
    
    def handle_greater_equal_than(self, left, right):
        return BoolType(left >= right)

    def handle_occur_after(self, left, right):
        return BoolType(left.timestamp > right.timestamp)
            
    def handle_occur_before(self, left, right):
        return BoolType(left.timestamp < right.timestamp)
    
    def handle_minus(self, left, right):
        return left - right
    
    def handle_plus(self, left, right):
        return left + right
    
    def handle_times(self, left, right):
        return left * right
    
    def handle_divide(self, left, right):
        return left / right
    
    def handle_list_shorthand(self, left, right):
        if isinstance(left, NumType) and isinstance(right,NumType) and left < right:
            list = ListType(None)
            for i in range (int(left.value), int(right.value) + 1):
                list.append(NumType(i))
            return list
        else:
            return NullType(None)


    def handle_string_concat(self, left, right):
        if isinstance(left, StrType) or isinstance(right, StrType):
            return StrType(left) + StrType(right)
        else:
            return NullType(None)
        
    def handle_where(self, left, right):
        if isinstance(right, BoolType):
            if right.bool_value() == True:
                return left

    # Ternary     
    def handle_is_within(self, value, min, max):
        return BoolType((value >= min).bool_value() and (value <= max).bool_value())   
    
    
    # Helper functions   
    def evaluate_params(self, node):
        evaluated_params = []
        for operation_node in node['arg']:
            if operation_node["type"] == "IT":
                evaluated_params.append(self.it)
            else:
                evaluated_params.append(self.eval_node(operation_node))
        return evaluated_params

    def debug_symbol_list(self):
        print(self.symbol_table) if self.debug else None

    def is_operator_node(self, node):
        return True if node["type"] in self.allowed_operations else False
    
    def is_terminal_node(self, node):
        if isinstance(node, list):
            return False
        elif node["type"] in TERMINAL_NODE_NAMES:
            return True
        else:
            return False
        
    def is_list(self, datatype):
        return True if isinstance(datatype, ListType) else False
        