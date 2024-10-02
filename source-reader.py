from tokenizer import Tokenizer

file_path = 'tokenizer-source.txt'

tokenizer = Tokenizer(file_path)

tokens = tokenizer.get_tokens()

for token in tokens:
    print(token)