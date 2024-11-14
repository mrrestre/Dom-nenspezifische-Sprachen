import json
import math

from data_types import *
from help_classes import *

class Interpreter:
    def __init__(self, path):
        self.debug = True
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
            "TRACE": self.handle_trace
        }

        self.operation_handlers = {            
            "PLUS": self.handle_plus,
            "MINUS": self.handle_minus,
            "SIN": self.handle_sin,
            "AMPERSAND": self.handle_string_concat
        }

        self.allowed_operations = list(self.operation_handlers.keys()) 

    def interpret(self):
        self.eval_node(self.ast)

    def eval_node(self, node):
        if is_terminal_node(node):
            return TerminalNode(node).get_value()
        elif self.is_operator_node(node):
            evaluated_params = []
            for operation_node in node['arg']:
                evaluated_params.append(self.eval_node(operation_node))

            operation_handler = self.operation_handlers.get(node["type"])
            match len(evaluated_params):
                case 1:
                    # Unary operation
                    # TODO: This should be better
                    result = ListType(None)
                    if node["type"] == 'SIN' and isinstance(evaluated_params[0], ListType):
                        for list_member in evaluated_params[0]:
                            result.append(operation_handler(list_member))
                    return result
                case 2:
                    # Binary operation
                    return operation_handler(evaluated_params)
                case 3:
                    # Terniary operation
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
        symbol_var = self.symbol_table[node['varname']] 
        if symbol_var:
            self.symbol_table[node['varname']].value = variable.value
        else:
            self.symbol_table[node['varname']] = variable
        self.debug_symbol_list()
            
    def handle_assign_time(self, node):
        timestamp = self.eval_node(node["arg"])
        self.symbol_table[node['varname']].timestamp = timestamp
        self.debug_symbol_list()
    
    def handle_identifier(self, node):
        variable = self.symbol_table[node['name']]
        self.debug_symbol_list()
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

    ## Expresions
    # Operations
    def handle_plus(self, operator_args):
        return operator_args[0] + operator_args[1]
    
    def handle_minus(self, operator_args):
        return operator_args[0] - operator_args[1]
    
    def handle_sin(self, operator_args):
        if isinstance(operator_args, NumType):
            return NumType(math.sin(operator_args.value))
        else:
            return NullType(None)
    
    def handle_string_concat(self, operator_args):
        left, right = operator_args[0], operator_args[1]
        if isinstance(left, StrType) or isinstance(right, StrType):
            return StrType(left) + StrType(right)
        else:
            return NullType(None)
    
    # Helper functions   
    def debug_symbol_list(self):
        print('[DEBUG] ', self.symbol_table) if self.debug else None

    def is_operator_node(self, node):
        return True if node["type"] in self.allowed_operations else False







