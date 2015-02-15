import copy

def lfsrFeedback(lfsr):
    bitIndexs = [7, 38, 70, 81, 96]
    feedbackBit = copy.copy(lfsr[0])
    for bitIndex in bitIndexs:
        feedbackBit ^= lfsr[bitIndex]
    return feedbackBit

def nfsrFeedback(nfsr, lfsr):
    return lfsr[0]^nfsr[0]^nfsr[26]^nfsr[56]^nfsr[91]^nfsr[96]\
           ^nfsr[3]&nfsr[67]\
        ^nfsr[11]&nfsr[13]\
        ^nfsr[17]&nfsr[18]\
        ^nfsr[27]&nfsr[59]\
        ^nfsr[40]&nfsr[48]\
        ^nfsr[61]&nfsr[65]\
        ^nfsr[68]&nfsr[84]\
        ^nfsr[88]&nfsr[92]&nfsr[93]&nfsr[95]\
        ^nfsr[22]&nfsr[24]&nfsr[25]\
        ^nfsr[70]&nfsr[78]&nfsr[82]

def h(nfsr, lfsr):
    return nfsr[12]&lfsr[8]^lfsr[13]&lfsr[20]^nfsr[95]&lfsr[42]^lfsr[60]&lfsr[79]^nfsr[12]&nfsr[95]&lfsr[94]

def preoutput(h, nfsr, lfsr):
    nfsrIndexs = [15, 36, 45, 64, 73, 89]
    nfsrBit = copy.copy(nfsr[2])
    for index in nfsrIndexs:
        nfsrBit ^= nfsr[index]
    return h^lfsr[93]^nfsrBit

def shiftState(state, newBit):
    state.pop(0)
    state.append(newBit)

def clock(nfsr, lfsr, production):
    print("nfsr state:")
    print(''.join(str(e) for e in nfsr[::-1]))
    print("lfsr state:")
    print(''.join(str(e) for e in lfsr[::-1]))
    hBit = h(nfsr, lfsr)
    print("hBit:")
    print(hBit)
    keyBit = preoutput(hBit, nfsr, lfsr)
    print("keyBit:")
    print(keyBit)
    nfsrFeedbackBit = nfsrFeedback(nfsr, lfsr)
    print("nfsrFeedbackBit:")
    print(nfsrFeedbackBit)
    lfsrFeedbackBit = lfsrFeedback(lfsr)
    print("lfsrFeedbackBit:")
    print(lfsrFeedbackBit)
    if production:
        shiftState(nfsr, nfsrFeedbackBit)
        shiftState(lfsr, lfsrFeedbackBit)
    else:
        shiftState(nfsr, nfsrFeedbackBit^keyBit)
        shiftState(lfsr, lfsrFeedbackBit^keyBit)
    return keyBit


def main():

    #keyVector = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    #ivVector =  "01111111111111111111111111111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    keyVector =  "00000001001000110100010101100111100010011010101111001101111011110001001000110100010101100111100010011010101111001101111011110000"
    ivVector =   "01111111111111111111111111111111000000010010001101000101011001111000100110101011110011011110111100010010001101000101011001111000"


    nfsr = [int(x) for x in keyVector[::-1]]
    lfsr = [int(x) for x in ivVector[::-1]]

    print(nfsr)
    print(len(nfsr))
    print(len(lfsr))
    for i in range(0, 256):
        print("init clock num "+str(i))
        clock(nfsr, lfsr, False)
    keyBits = []
    for i in range(0, 40):
        print("clock num "+str(i))
        keyBits.append(clock(nfsr, lfsr, True))
    print("bits: ")
    keyOut = ""
    for i in range(0,40,4):
        hexNum = [keyBits[i],keyBits[i+1],keyBits[i+2],keyBits[i+3]]
        keyOut+=hex(int('0b'+''.join(str(e) for e in hexNum), 2))[2:]
    print(keyOut)
if __name__ == "__main__":
    main()
