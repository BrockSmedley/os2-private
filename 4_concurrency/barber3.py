from enum import Enum
from multiprocessing import Process, Queue
import math
from random import randint, shuffle
import time

class Barbershop:
    def __init__(self, n):
        self.waitingRoom = Queue(n)
    
    def isFull(self):
        return self.waitingRoom.full()

    def isEmpty(self):
        return self.waitingRoom.empty()

    def serveNextCustomer(self):
        customer = self.waitingRoom.get()
        customer.getHaircut(randint(1,3))
        customer.goHome()

    def addCustomer(self, customer):
        if (not self.isFull()):
            print (customer.name, "joining waiting room.")
            self.waitingRoom.put(customer)
        else:
            print ("Waiting room is full.")
            customer.goHome()

    def sleep(self):
        if (self.isEmpty()):
            print ("Barber is sleeping.")
            # block until next customer arrives
            try:
                customer = self.waitingRoom.get(timeout=6)
                return customer
            except Exception as e:
                return None
        else:
            return None

    def wakeBarber(self):
        print ("Barber waking up.")

class Customer:
    def __init__(self, name):
        self.name = name
        self.longHair = True

    def gotoBarber(self, barbershop):
        if (barbershop.isFull()):
            print ("Barbershop is full.")
            self.goHome()
        elif (not barbershop.isEmpty() and not barbershop.isFull()):
            barbershop.addCustomer(self)
        elif (barbershop.isEmpty()):
            barbershop.wakeBarber()
            barbershop.addCustomer(self)

    def getHaircut(self, t):
        # called by barber only
        # barber blocks until this finishes
        print ("Cutting %s's hair." % self.name)
        time.sleep(t)
        self.longHair = False

    def goHome(self):
        # called by barber only
        print (self.name, "is going home.")


def getNames(c):
    names = ["buddy", "guy", "friend"]
    cnames = []
    for nn in range(c):
        cnames.append(names[nn%len(names)] + " " + str(math.floor(nn/len(names)+1)))
    return cnames

# generates customers and tries to send them to the barbershop
def generateCustomers(barbershop, cnames):
    for c in cnames:
        customer = Customer(c)
        p = Process(target=customer.gotoBarber, args=(barbershop,))
        p.start()
        p.join() # need for barber state (sleep) preservation
        cnames.remove(c)
    # returns remaining names
    return cnames

def feedCustomers(barbershop, cnames):
    while (len(cnames) > 0):
        cnames = generateCustomers(barbershop, cnames)

def serveCustomers(barbershop):
    while (not barbershop.isEmpty()):
        barbershop.serveNextCustomer()

def main():
    n = 2
    barbershop = Barbershop(n)

    # make up 5 customers
    cnames = getNames(5)

    while (True):
        # asynchronously add customers to waiting room
        f = Process(target=feedCustomers, args=(barbershop,cnames))
        f.start()
        f.join()

        # synchronously process haircuts
        p = Process(target=serveCustomers, args=(barbershop,))
        p.start()
        p.join()
        
        # barbershop now empty; wait for new customer
        customer = barbershop.sleep()
        if (customer):
            #cnames.append(customer.name)
            print ("hmmm")

        shuffle(cnames)

if __name__ == '__main__':
    main()