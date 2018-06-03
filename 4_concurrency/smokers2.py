# smokers.py
# Brock Smedley 2018 - All rights reserved

from enum import Enum
from multiprocessing import Process, Queue
import math
from random import randint, shuffle
import time

class Resource(Enum):
    TOBACCO = 1
    PAPER = 2
    MATCHES = 3


class Smoker:
    def __init__(self, resource, name):
        self.availableResource = resource
        self.name = name

    # tries to get resources from Agent, claims them if a stoog can be rolled
    def tryGetResources(self, agent):
        # get list of offered ingredients from "Queue"
        ingredients = agent.chosenIngredients.get()
        # if stoog can be rolled, claim the resources
        if (self.availableResource not in ingredients):
            agent.giveResources(self)
        # otherwise, put the ingredients back in the queue; let someone else try
        else:
            agent.chosenIngredients.put(ingredients)

    # just a print function about rolling a stoog.
    def roll(self):
        print (self.name, "has %s" % str(self.availableResource).replace("Resource.",''), "and is now rolling a stoog.")

    # main smoker loop; smokers continuously fiend for stoogs, on every second they try to roll one.
    def fiend(self, agent):
        smoking = True
        bad = True
    
        while (smoking == bad):
            print ("mkay")
            self.tryGetResources(agent)
            time.sleep(1)


class Agent:
    def __init__(self):
        self.chosenIngredients = Queue(1) # single-item Queue to hold list of two chosen resources to give out
        self.chooseNewIngredients() # start by choosing a fresh random set of ingredients
        # agent is NOT aware of smokers' state/resource requirements

    # chooses a new random set of two ingredients
    # called after a smoker claims some ingredients
    def chooseNewIngredients(self):
        rNames = [Resource.TOBACCO, Resource.PAPER, Resource.MATCHES]
        i1 = randint(0,2)
        i2 = randint(0,2)
        if (i2 == i1):
            i2 = (i1 + 1) % 3

        # make the resources look nice
        ingredients = [rNames[i1], rNames[i2]]
        zzz = []
        for i in ingredients:
            zzz.append(str(i).replace("Resource.",''))
        print ("AGENT offering %s and %s" % (zzz[0],zzz[1]))

        # empty out chosenIngredients "queue"
        while (not self.chosenIngredients.empty()):
            self.chosenIngredients.get(block=False)
        # put new selection of ingredients in
        self.chosenIngredients.put(ingredients)

    # choose which smoker gets the two ingredients
    # called by Smoker only!
    def giveResources(self, smoker):
        smoker.roll()
        self.chooseNewIngredients()


def main():
    agent = Agent()
    rNames = [Resource.TOBACCO, Resource.PAPER, Resource.MATCHES]
    sNames = ["Bob", "Bill", "Bud"]
    smokers = []
    # make the smokers with their respective resources
    for i in range(len(rNames)):
        smokers.append(Smoker(rNames[i], sNames[i]))
        print ("SMOKER: ", smokers[i].name, "has", smokers[i].availableResource)

    # start all the smoker fiending processes
    for s in smokers:
        p = Process(target=s.fiend, args=(agent,))
        p.start()
        # don't join() the processes; we don't want to block on any single one of them
    
if __name__ == '__main__':
    main()