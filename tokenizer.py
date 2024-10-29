import re
import json

class Tokenizer:
    def __init__(self, input_file_path, token_file_path):
        self.input_file_path = input_file_path
        self.output_file_path = token_file_path
        self.tokens = []

        self.reserved_words = [ 'READ', 'WRITE', 'IF', 'THEN', 'ELSEIF', 'ELSE', 'ENDIF', 'FOR',
                                'TIME', 'OF', 'NOW',
                                'IN', 'DO', 'ENDDO', 'NOW', 'CURRENTTIME', 'MINIMUM', 'MAXIMUM',
                                'FIRST', 'LAST', 'SUM', 'AVERAGE', 'EARLIEST', 'LATEST', 'SIN', 'COS']
        self.symbols = {
            ':=':   'ASSIGN',
            '**':   'POWER',
            '<=':   'LTEQ', 
            '=>':   'GTEQ',
            '<>':   'NEQ',

            ';':    'SEMICOLON', 
            '=':    'EQ', 
            ',':    'COMMA',
            '+':    'PLUS',
            '-':    'MINUS',
            '*':    'TIMES',  
            '/':    'DIVIDE',
            '(':    'LPAR', 
            ')':    'RPAR', 
            '[':    'LSPAR', 
            ']':    'RSPAR',
            '{':    'LKPAR', 
            '}':    'RKPAR',
            '&':    'AMPERSAND',
            '<':    'LT',
            '>':    'GT'
        }

        self.regex = {
            'IDENTIFIER' :  r'^[a-zA-Z][a-zA-Z0-9_]*\b',
            'TIMETOKEN':    r'^\d{4}-\d{2}-\d{2}(?:T\d{2}:\d{2}:\d{2})?\b',
            'NUMTOKEN':     r'\b\d+(\.\d+)?\b',
            'STRTOKEN':     r'^["\'](.*?)["\']'
        }

        self.tokenize_file()

    def tokenize_file(self):
        with open(self.input_file_path, 'r') as file:
            for line_number, line in enumerate(file, start=1):
                tokens = self.tokenize_line(line)
                for token, category, column in tokens:
                    # temp_json = [str(line_number), str(column), category, token]
                    temp_json = [str(line_number), category, token]

                    self.tokens.append(temp_json)
        # Save to a JSON file
        with open(self.output_file_path, 'w') as file:
            json.dump(self.tokens, file)

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
                lower_line = line.lower()
                if lower_line.startswith(reserved_word.lower()):
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
                    value = result.group(1) if description == 'STRTOKEN' else result.group()
                    found = True
                    categorized_tokens.append((value, description, column))  
                    column += len(result.group(0))
                    line = line[len(result.group(0)):]
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