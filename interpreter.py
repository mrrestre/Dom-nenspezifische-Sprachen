import json
import datetime

from myDataTypes import *

class Interpreter:
    def __init__(self, path):
        self.path = path
        self.variables = {}
        self.ast = {}
        self.now = datetime.datetime.now()

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
            "NOW": self.handle_now,
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
        value = self.eval_node(node["arg"])
        variable = self.variables.get(node['varname'])
        if variable:
            variable_tuple = {"value": value, "timestamp": variable["timestamp"]}
            self.variables.update({node['varname']: variable_tuple})
        else:
            variable_tuple = {"value": value, "timestamp": None}
            self.variables.update({node['varname']: variable_tuple})
    
    def handle_assign_time(self, node):
        timestamp = self.eval_node(node["arg"])
        variable = self.variables.get(node['varname'])
        if variable:
            variable_tuple = {"value": variable["value"], "timestamp": timestamp}
            self.variables.update({node['varname']: variable_tuple})
        else:
            variable_tuple = {"value": None, "timestamp": timestamp}
            self.variables.update({node['varname']: variable_tuple})
    
    def handle_identifier(self, node):
        variable = self.variables.get(node['name'])
        if variable:
            return variable
        else: 
            return 'Variable ' + node['name'] + ' in line ' + node['line'] + ' not defined in scope'

    def handle_write(self, node):
        result = self.eval_node(node["arg"])
        if type(result) != dict:
            print(result)
        else:
            print(result['value'])
    
    def handle_write_time(self, node):
        result = self.eval_node(node["arg"])
        print(result['timestamp'])

    def handle_trace(self, node):
        line = node["line"]
        result = self.eval_node(node["arg"])
        if type(result) != dict:
            print("Line", line, "-", result)
        else:
            print("Line", line, "-", result['value'])

    ## Expresions
    # Operations
    def handle_plus(self, node):
        args = node["arg"]
        return self.eval_node(args[0]) + self.eval_node(args[1])
    
    def handle_minus(self, node):
        args = node["arg"]
        return self.eval_node(args[0]) - self.eval_node(args[1])
    
    def handle_string_concat(self, node):
        args = node["arg"]
        return self.eval_node(args[0]) + self.eval_node(args[1])
    
    # Terminal nodes
    def handle_str_token(self, node):
        return node["value"]

    def handle_num_token(self, node):
        return float(node["value"])
    
    def handle_time_token(self, node):
        return node["value"]
    
    def handle_now(self, node):
        return self.now






