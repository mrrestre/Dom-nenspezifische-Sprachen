class NumType():
    def __init__(self, value=None, timestamp=None):
        self.value = float(value)
        self.timestamp = timestamp
    def __str__(self):
        return str(self.value)