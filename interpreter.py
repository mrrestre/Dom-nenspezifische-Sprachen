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

        with open(path, 'r') as file:
            self.ast = json.load(file)

        self.statement_handlers = {
            "ASSIGN": self.handle_assign,
            "ASSIGN_TIME": self.handle_assign_time,
            "IDENTIFIER": self.handle_identifier,
            "STATEMENTBLOCK": self.handle_statement_block,
            "TRACE": self.handle_trace,
            "TRACE_TIME": self.handle_trace_time,
            "WRITE": self.handle_write,
            "WRITE_TIME": self.handle_write_time,
        }

        self.unary_member_operations = {
            "IS_NUMBER": self.handle_is_number,
            "SIN": self.handle_sin,
            "NOT": self.handle_not,
        }

        self.unary_list_operations = {
            "COUNT": self.handle_count,
            "FIRST": self.handle_first,
            "IS_LIST": self.handle_is_list,
            "IS_NOT_LIST": self.handle_is_not_list,
        }

        self.unary_operations = self.unary_member_operations | self.unary_list_operations

        self.binary_operations = {
            "AMPERSAND": self.handle_string_concat,
            "MINUS": self.handle_minus,
            "PLUS": self.handle_plus,
            "WHERE": self.handle_where,
        }

        self.ternary_operations = {
        }

        self.operation_handlers = self.unary_operations | self.binary_operations | self.ternary_operations

        self.allowed_operations = list(self.operation_handlers.keys()) 

    def interpret(self):
        self.eval_node(self.ast)

    def eval_node(self, node):
        if self.is_terminal_node(node):
            return TerminalNode(node).get_value()
        elif self.is_operator_node(node):
            evaluated_params = self.evaluate_params(node)
            operation_handler = self.operation_handlers.get(node["type"])

            match len(evaluated_params):
                # Unary operation
                case 1:
                    argument = evaluated_params[0]

                    if node["type"] in self.unary_member_operations:
                        if isinstance(argument, ListType):
                            result = ListType(None)
                            for list_member in argument:
                                result.append(operation_handler(list_member))
                            return result
                        else:
                            return operation_handler(argument)
                    elif node["type"] in self.unary_list_operations:
                        return operation_handler(argument)

                # Binary operation
                case 2:
                    left = evaluated_params[0]
                    right = evaluated_params[1]

                    # Both are lists
                    if isinstance(left, ListType) and isinstance(right, ListType):
                        if len(left) == len(right):
                            # Perform the operation position based: result[i] = left[i] <operation> right[i]
                            result = ListType(None)
                            for i in range(len(left)):
                                result.append(operation_handler(left[i], right[i]))
                            return result
                        else:
                            return NullType(None)
                    # Only right is a list
                    elif not isinstance(left, ListType) and isinstance(right, ListType):
                        # Perform the operation on left with all elements of right: result[i] = left <operation> right[i]
                        result = ListType(None)
                        for i in range(len(right)):
                            result.append(operation_handler(left, right[i]))
                        return result
                    # Only left is a list
                    elif isinstance(left, ListType) and not isinstance(right, ListType):
                        # Perform the operation on left with all elements of right: result[i] = left[i] <operation> right
                        result = ListType(None)
                        for i in range(len(left)):
                            result.append(operation_handler(left[i], right))
                        return result
                    # Neither arg is a list
                    elif not isinstance(left, ListType) and not isinstance(right, ListType):
                        return operation_handler(left, right)
                
                # Ternary operation
                case 3:
                    return operation_handler(evaluated_params)
        else:
            handler = self.statement_handlers.get(node["type"])
            if not handler:
                raise ValueError(f"Handler not found for node type: {node["type"]}")
            return handler(node)

    # Statement Block
    def handle_statement_block(self, node):
        for statement in node["statements"]:
            self.eval_node(statement)

    # Statements
    def handle_assign(self, node):
        variable = self.eval_node(node["arg"])
        if self.symbol_table.isKeyPresent(node['varname']):
            self.symbol_table[node['varname']].value = variable.value
        else:
            self.symbol_table.update({node['varname']: variable.copy()})
        self.debug_symbol_list()
            
    def handle_assign_time(self, node):
        timestamp = self.eval_node(node["arg"])
        self.symbol_table[node['varname']].timestamp = timestamp.value
        self.debug_symbol_list()
    
    def handle_identifier(self, node):
        if self.symbol_table.isKeyPresent(node['name']):
            return self.symbol_table[node['name']]
        else:
            return 'Variable ' + node['name'] + ' in line ' + node['line'] + ' not defined in scope'

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
        if isinstance(arg, ListType):
            return NumType(len(arg))
        else:
            return NullType(None)
    
    def handle_first(self, arg):
        if isinstance(arg, ListType):
            return NumType(arg[0])
        else:
            return NullType(None)
        
    def handle_is_list(self, arg):
        if isinstance(arg, ListType):
            return BoolType('true')
        else:
            return BoolType('false')
        
    def handle_is_not_list(self, arg):
        return BoolType(self.handle_is_list(arg).negate())

    # Binary
    def handle_plus(self, left, right):
        return left + right
    
    def handle_minus(self, left, right):
        return left - right
    
    def handle_string_concat(self, left, right):
        if isinstance(left, StrType) or isinstance(right, StrType):
            return StrType(left) + StrType(right)
        else:
            return NullType(None)
        
    def handle_where(self, left, right):
        if isinstance(right, BoolType):
            if right.bool_value() == True:
                return left
    
    # Helper functions   
    def evaluate_params(self, node):
        evaluated_params = []
        for operation_node in node['arg']:
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