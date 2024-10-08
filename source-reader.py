import json
from tokenizer import Tokenizer

file_path = 'tokenizer-source.txt'

tokenizer = Tokenizer(file_path)

tokens = tokenizer.get_tokens()

json_string = json.dumps(tokens, indent=4)
print(json_string)

# for token in tokens:
#     print(token)