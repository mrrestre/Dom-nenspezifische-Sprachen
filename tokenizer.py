import re

class Tokenizer:
    def __init__(self, file_path):
        self.file_path = file_path
        self.tokens = []

        self.reserved_words = [ 'READ', 'WRITE', 'IF', 'THEN', 'ELSEIF', 'ELSE', 'ENDIF', 'FOR',
                                'IN', 'DO', 'ENDDO', 'NOW', 'CURRENTTIME', 'MINIMUM', 'MAXIMUM',
                                'FIRST', 'LAST', 'SUM', 'AVERAGE', 'EARLIEST', 'LATEST']
        self.symbols = {
            ':=':   'ASSIGN',
            '**':   'POWER',
            ';':    'SEMICOLON', 
            '<=':   'LTEQ', 
            '=>':   'GTEQ',
            '<>':   'NEQ',

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
            'STRLITERAL':   r'^["\'].*?["\']'
        }

        self.tokenize_file()

    def tokenize_file(self):
        with open(self.file_path, 'r') as file:
            for line_number, line in enumerate(file, start=1):
                tokens = self.tokenize_line(line)
                for token, category, column in tokens:
                    temp_json = [str(line_number), str(column), category, token]

                    self.tokens.append(temp_json)

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