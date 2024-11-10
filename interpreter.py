import json

from myDataTypes import *
from helpClasses import SymbolTable

class Interpreter:
    def __init__(self, path):
        self.path = path
        self.symbol_table = SymbolTable()
        self.ast = {}
        self.now = datetime.now()

        with open(self.path, 'r') as file:
            self.ast = json.load(file)

        self.node_type_handlers = {
            "STATEMENTBLOCK": self.handle_statement_block,

            "ASSIGN": self.handle_assign,
            "ASSIGN_TIME": self.handle_assign_time,
            "IDENTIFIER": self.handle_identifier,
            "WRITE": self.handle_write,
            "WRITE_TIME": self.handle_write_time,
            "TRACE": self.handle_trace,
            
            "PLUS": self.handle_plus,
            "MINUS": self.handle_minus,
            "AMPERSAND": self.handle_string_concat,

            "STRTOKEN": self.handle_str_token,
            "NUMTOKEN": self.handle_num_token,
            "TIMETOKEN": self.handle_time_token,
            "BOOLTOKEN": self.handle_bool_token,
            "NOW": self.handle_now,
            "LIST": self.handle_list,
            "EMPTYLIST": self.handle_empty_list,
        }

    def interpret(self):
        self.eval_node(self.ast)

    def eval_node(self, node):
        node_type = node["type"]
        handler = self.node_type_handlers.get(node_type)
        if not handler:
            raise ValueError(f"Handler not found for node type: {node_type}")
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
            
    def handle_assign_time(self, node):
        timestamp = self.eval_node(node["arg"])
        self.symbol_table[node['varname']].timestamp = timestamp
    
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

    ## Expresions
    # Operations
    def handle_plus(self, node):
        left, right = self.abstract_args(node)
        return left + right
    
    def handle_minus(self, node):
        left, right = self.abstract_args(node)
        return left - right
    
    def handle_string_concat(self, node):
        left, right = self.abstract_args(node)
        if isinstance(left, StrType) or isinstance(right, StrType):
            return StrType(left) + StrType(right)
        else:
            return None
    
    # Terminal nodes
    def handle_str_token(self, node):
        return StrType(node["value"])

    def handle_num_token(self, node):
        return NumType(node["value"])
    
    def handle_time_token(self, node):
        return DateType(node["value"])
    
    def handle_bool_token(self, node):
        return BoolType(node["value"])
    
    def handle_now(self, node):
        return self.now
    
    def handle_list(self, node):
        return ListType(node["elements"])
    
    def handle_empty_list(self, node):
        return ListType(None)
    
    # Helper functions
    def abstract_args(self, node):
        args = []
        node_args = node["arg"]
        if len(node_args) == 1:
            return self.eval_node(node_args[0])
        elif len(node_args) == 2:
            return self.eval_node(node_args[0]), self.eval_node(node_args[1])
        elif len(node_args) == 3:
            return self.eval_node(node_args[0]), self.eval_node(node_args[1]), self.eval_node(node_args[2])
        else:
            return None







