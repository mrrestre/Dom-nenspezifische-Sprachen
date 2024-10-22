import json

class Interpreter:
    def __init__(self, path):
        self.path = path
        self.statements = []
        self.load_compiled()

        self.functions = {
            'WRITE': self.do_write,
        }
        self.expressions = {
            'STRTOKEN': self.strtoken,
            'NUMTOKEN': self.numtoken,
            'PLUS': self.plus,
        }

    def load_compiled(self):
        with open(self.path, 'r') as file:
            compiled = json.load(file)
            if compiled['type'] == 'STATEMENTBLOCK':
                self.statements = compiled['statements']
            else:
                raise ValueError("Invalid code structure")

    def interpret(self):
        for statement in self.statements:
            self.process_statement(statement)

    def process_statement(self, statement):
        statement_type = statement['type']
        # Search for the function in the library
        function = self.functions.get(statement_type)
        if function:
            # Execute the function with args
            function(statement['arg'])
        else:
            print(f"Unknown statement type: {statement_type}")

    def do_write(self, arg):
        arg_type = arg['type']
        expression = self.expressions.get(arg_type)
        if expression:
            if arg_type == 'PLUS':
                result = expression(arg['arg'])
                print(result)
            else:
                print(expression(arg))
        else:
            print(f"Unknown argument type: {arg_type}")

    def strtoken(self, arg):
        return arg['value']

    def numtoken(self, arg):
        return arg['value']

    def plus(self, args):
        total = 0
        for arg in args:
            arg_type = arg['type']
            if arg_type == 'NUMTOKEN':
                total += float(arg['value'])
            else:
                print(f"Unknown argument type in PLUS: {arg_type}")
        return total