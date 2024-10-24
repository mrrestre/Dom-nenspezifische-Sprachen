import json

class Interpreter:
    def __init__(self, path):
        self.path = path
        self.variables = {}
        self.ast = {}
        self.load_ast()

        self.node_type_handlers = {
            "STATEMENTBLOCK": self.handle_statement_block,

            "ASSIGN": self.handle_assign,
            "IDENTIFIER": self.handle_identifier,
            "WRITE": self.handle_write,
            
            "PLUS": self.handle_plus,
            "MINUS": self.handle_minus,

            "STRTOKEN": self.handle_str_token,
            "NUMTOKEN": self.handle_num_token,
        }

    def load_ast(self):
        with open(self.path, 'r') as file:
            self.ast = json.load(file)

    def interpret(self):
        ast = self.parse_node(self.ast)
        self.visit(ast)

    def parse_node(self, node_data):
        node_type = node_data["type"]
        if node_type not in self.node_type_handlers:
            raise ValueError(f"Unknown node type: {node_type}")
        return node_data

    def visit(self, node):
        node_type = node["type"]
        handler = self.node_type_handlers.get(node_type)
        if not handler:
            raise ValueError(f"Handler not found for node type: {node_type}")
        return handler(node)

    def handle_statement_block(self, node):
        for statement in node["statements"]:
            self.visit(statement)

    def handle_assign(self, node):
        result = self.visit(node["arg"])
        self.variables.update({node['varname']: result})
    
    def handle_identifier(self, node):
        variable_name = self.variables.get(node['name'])
        if variable_name:
            return variable_name
        else: 
            return 'Variable ' + node['name'] + ' in line ' + node['line'] + ' not defined in scope'

    def handle_write(self, node):
        result = self.visit(node["arg"])
        print(result)


    def handle_plus(self, node):
        args = node["arg"]
        return self.visit(args[0]) + self.visit(args[1])
    
    def handle_minus(self, node):
        args = node["arg"]
        return self.visit(args[0]) - self.visit(args[1])
    

    def handle_str_token(self, node):
        return node["value"]

    def handle_num_token(self, node):
        return float(node["value"])






