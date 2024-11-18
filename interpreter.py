import json
import math

from data_types import *
from help_classes import *

class Interpreter:
    def __init__(self, path):
        self.debug = False
        self.path = path
        self.symbol_table = SymbolTable()
        self.ast = {}
        self.now = datetime.now()

        with open(self.path, 'r') as file:
            self.ast = json.load(file)

        self.statement_handlers = {
            "STATEMENTBLOCK": self.handle_statement_block,
            "ASSIGN": self.handle_assign,
            "ASSIGN_TIME": self.handle_assign_time,
            "IDENTIFIER": self.handle_identifier,
            "WRITE": self.handle_write,
            "WRITE_TIME": self.handle_write_time,
            "TRACE": self.handle_trace,
            "TRACE_TIME": self.handle_trace_time,
        }

        self.unary_member_operations = {
            "SIN": self.handle_sin,
            "IS_NUMBER": self.handle_is_number,
        }

        self.unary_list_operations = {
            "COUNT": self.handle_count,
            "FIRST": self.handle_first,
            "IS_LIST": self.handle_is_list,
            "IS_NOT_LIST": self.handle_is_not_list,
        }

        self.unary_operations = self.unary_member_operations | self.unary_list_operations

        self.binary_operations = {
            "PLUS": self.handle_plus,
            "MINUS": self.handle_minus,
            "AMPERSAND": self.handle_string_concat
        }

        self.ternary_operations = {
        }

        self.operation_handlers = self.unary_operations | self.binary_operations | self.ternary_operations

        self.allowed_operations = list(self.operation_handlers.keys()) 

    def interpret(self):
        self.eval_node(self.ast)

    def eval_node(self, node):
        if is_terminal_node(node):
            return TerminalNode(node).get_value()
        elif self.is_operator_node(node):
            evaluated_params = self.evaluate_params(node)
            operation_handler = self.operation_handlers.get(node["type"])

            match len(evaluated_params):
                case 1:
                    # Unary operation
                    if node["type"] in self.unary_member_operations:
                        if(isinstance(evaluated_params[0], ListType)):
                            result = ListType(None)
                            for list_member in evaluated_params[0]:
                                result.append(operation_handler(list_member))
                        else:
                            result = operation_handler(evaluated_params[0])
                    elif node["type"] in self.unary_list_operations:
                        result = operation_handler(evaluated_params[0])
                    return result
                case 2:
                    # Binary operation
                    return operation_handler(evaluated_params)
                case 3:
                    # Ternary operation
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
        variable = self.symbol_table[node['name']]
        return variable if variable else 'Variable ' + node['name'] + ' in line ' + node['line'] + ' not defined in scope'

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
    def handle_sin(self, operator_args):
        if isinstance(operator_args, NumType):
            return NumType(math.sin(operator_args.value))
        else:
            return NullType(None)
    
    def handle_is_number(self, operator_args):
        if isinstance(operator_args, NumType):
            return BoolType('true')
        else:
            return BoolType('false')
    
    def handle_count(self, operator_args):
        return len(operator_args)  
    
    def handle_first(self, operator_args):
        return operator_args[0]
        
    def handle_is_list(self, operator_args):
        if isinstance(operator_args, ListType):
            return BoolType('true')
        else:
            return BoolType('false')
        
    def handle_is_not_list(self, operator_args):
        if isinstance(operator_args, ListType):
            return BoolType('false')
        else:
            return BoolType('true')

    # Binary
    def handle_plus(self, operator_args):
        return operator_args[0] + operator_args[1]
    
    def handle_minus(self, operator_args):
        return operator_args[0] - operator_args[1]
    
    def handle_string_concat(self, operator_args):
        left, right = operator_args[0], operator_args[1]
        if isinstance(left, StrType) or isinstance(right, StrType):
            return StrType(left) + StrType(right)
        else:
            return NullType(None)
    
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







