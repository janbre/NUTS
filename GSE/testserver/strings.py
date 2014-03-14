class Prefix:
    HAL = 'HAL'
    WELCOME = 'welcome'
    HELP = 'help'
    QUIT = 'quit'

class Message:
    HAL = '''ALL THESE WORLDS ARE YOURS EXCEPT EUROPA\nATTEMPT NO LANDING THERE\n'''
    WELCOME = 'This is your reptillian overlords, what can we help you with today?\n'
    HELP_REQUEST = 'Received request for help, transmitting hydrospanner manual'
    HELP_TEXT = '''Available commands:\nQuit:\tClose connection\nHelp:\tPrints this message\nStart:\tBegin transmission\n'''
    QUIT = 'Good day to you, madam/sir!\n'

class Info:
    WELCOME = '''Welcome to Alien Transmission Reception INC.\nNo ongoing transmissions. Would you like to receive one? (Y/N): '''

    NEGATIVE = '''Apparantly, E.T doesn\'t want to phone home today'''
    SHUT_DOWN = 'Shutting down'

class Error:
    CONNECTION = 'Unable to connect to server. Are you certain our reptillian overlords are trying to reach you?'

