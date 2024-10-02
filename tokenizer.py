import re

class Tokenizer:
    def __init__(self, file_path):
        self.file_path = file_path
        self.tokens = []

        self.reserved_words = ['FOR', 'IN', 'DO', 'WRITE', 'ENDDO']
        self.symbols = {
            ';':    'SEMICOLON', 
            '=':    'EQUAL', 
            '(':    'LPAR', 
            ')':    'RPAR', 
            '{':    'LSPAR', 
            '}':    'LRPAR', 
            '+':    'PLUS', 
            '-':    'MINUS', 
            '*':    'MULT',  
            ':=':   'ASSIGN', 
            '[':    'LSPAR', 
            ']':    'RSPAR',
            ',':    'COMMA'
        }

        self.regex = {
            'IDENTIFIER' :  r'^[a-zA-Z][a-zA-Z0-9]*\b',
            'NUMTOKEN':     r'\d',
            'STRLITERAL':   r'^["\'].*?["\']'
        }

        self.tokenize_file()

    def tokenize_file(self):
        with open(self.file_path, 'r') as file:
            for line_number, line in enumerate(file, start=1):
                tokens = self.tokenize_line(line)
                for token, category, column in tokens:
                    self.tokens.append((line_number, column, category, token))

    def tokenize_line(self, line):        
        categorized_tokens = []
        line = line.replace('\n', '')
        column = 1
        found = False

        while line:
            found = False
            line, white_spaces = self.remove_leading_whitespace(line)
            column += white_spaces
            
            for reserved_word in self.reserved_words:
                if line.startswith(reserved_word.lower()):
                    found = True
                    categorized_tokens.append((reserved_word, reserved_word, column))
                    column += len(reserved_word)
                    line = line[len(reserved_word):]
                    break

            for symbol, description in self.symbols.items():
                if line.startswith(symbol):
                    found = True
                    categorized_tokens.append((symbol, description, column))
                    column += len(symbol)
                    line = line[len(symbol):]
                    break
            
            for description, pattern in self.regex.items():
                result = re.match(pattern, line, re.I)
                if result:
                    value = result.group()
                    found = True
                    categorized_tokens.append((value, description, column))
                    column += len(value)
                    line = line[len(value):]
                    break
            
            if not found:
                categorized_tokens.append(('UNKNOWN', line, column))
                line = ''
        
        return categorized_tokens

    def remove_leading_whitespace(self, string):
        i = 0
        while i < len(string) and string[i].isspace():
            i += 1
        new_string = string[i:]
        erased_chars = i
        return new_string, erased_chars

    def get_tokens(self):
        return self.tokens