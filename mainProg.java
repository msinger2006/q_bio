// multi-threaded program to simulate drawing numbers and playing the lottery

import java.util.*;

class Lottery extends Thread {

    // max ball number (69), power ball number (26), and number of balls (5)
    private int maxBallNumber = 0;
    private int maxPowerBallNumber = 0;
    private int numberOfBalls = 0;

    // list holding numbers on balls (1-69)
    private List<Integer> listOfNumbers;

    // winning ball and power ball numbers
    private Vector<Integer> winBalls;
    private int powerBall;

    // number of draws (default value)
    private long numberOfDraws = 160000000;

    // results from a single draw
    private Vector<Integer> draw = new Vector<Integer>(6);

    // store winning seeds
    private Vector<Long> winningSeeds = new Vector<Long>();
    private long seed = 0;

    // running in test mode
    private boolean isTest = true;

    // multi-threading variables
    private int maxNumberOfThreads = 1;
    private int threadCount = 0;

    // number of combinations
    private long iCount = 0;

    // constructor
    public Lottery(int maxBallNumber, int maxPowerBallNumber,
		   int maxNumberOfThreads) {
	this.maxBallNumber = maxBallNumber;
	this.maxPowerBallNumber = maxPowerBallNumber;
	this.maxNumberOfThreads = maxNumberOfThreads;

	listOfNumbers = new ArrayList<Integer>(maxBallNumber);
    }

    // initialize and set winning numbers
    public void initializeGame(Vector<Integer> winBallsIn, int powerBall) {
	for(int i = 1; i <=maxBallNumber; i++) {
	    listOfNumbers.add(i);
	}

	numberOfBalls = winBallsIn.size();
	winBalls = new Vector<Integer>(winBallsIn);
	this.powerBall = powerBall;
    }

    // initialize for multi-threaded run
    public void initializeGame(Vector<Integer> winBallsIn, int powerBall,
			       int threadCount) {
	this.threadCount = threadCount;
	initializeGame(winBallsIn, powerBall);
    }

    // start playing
    public void run() {
	if(isTest) {
	    playGame();
	}
	else {
	    iCount = 0;
	    while(iCount < numberOfDraws) {
		Long temp1 = new Long(iCount);
		Long temp2 = new Long(maxNumberOfThreads);
		Long temp3 = new Long(temp1*temp2);
		Long seed = new Long(threadCount + temp3);
		playGame(seed);
		iCount++;
	    }
	}
    }

    // getter/setter methods
    public void setIsTest(boolean isTest) {
	this.isTest = isTest;
    }

    public void setNumberOfDraws(long numberOfDraws) {
	this.numberOfDraws = numberOfDraws;
    }

    public Vector<Long> getWinningSeeds() {
	return winningSeeds;
    }

    public long getNumberOfCombinations() {
	return iCount;
    }

    public Vector<Integer> getCombination(long seed) {
	Random random = new Random(seed);
	return generateOutcome(random);
    }

    // generate single outcome given a random object
    private Vector<Integer> generateOutcome(Random random) {
	int randomNumber, ball;
	Vector<Integer> balls = new Vector<Integer>(numberOfBalls);
	List<Integer> listOfNumbersCopy = new ArrayList<Integer>(listOfNumbers);

	for(int i = 0; i < numberOfBalls; i++) {
	    randomNumber = random.nextInt(maxBallNumber-i);
	    ball = listOfNumbersCopy.get(randomNumber);
	    listOfNumbersCopy.remove(randomNumber);
	    balls.addElement(ball);
	}
	randomNumber = random.nextInt(maxPowerBallNumber) + 1;
	balls.addElement(randomNumber);

	return balls;
    }

    // play game one time with input seed
    private void playGame(long seed) {
	Random random = new Random(seed);
	draw = generateOutcome(random);
	if(draw.lastElement() == powerBall) {
	    if(winBalls.containsAll(draw.subList(0,5))) {
		System.out.println("thread " + threadCount +
				   ", winning seed and combination = " + seed +
				   " : " + draw.subList(0,5) + " " +
				   draw.lastElement());
		winningSeeds.addElement(seed);
	    }
	}
    }

    // play game numberOfDraw times (test mode)
    private void playGame() {
	for(int i = 0; i < numberOfDraws; i++) {
	    long seed = threadCount + i*maxNumberOfThreads;
	    Random random = new Random(seed);
	    int iCount = 0;
	    while(true) {
		iCount++;
		draw = generateOutcome(random);
		if(draw.lastElement() == powerBall) {
		    if(winBalls.containsAll(draw.subList(0,5))) {
			System.out.println(seed + " : " + iCount +
					   " : " + draw);
			break;
		    }
		}
	    }
	}
    }

}


public class mainProg {
    public static void main(String[] args) {

	int maxNumberOfThreads = 1;
	double minutesToRun = .5;
	if(args.length < 1) {
	    System.out.println("Usage: mainProg [number of threads] " +
			       "[minutes to run]");
	}
	else if(args.length == 1) {
	    maxNumberOfThreads = Integer.valueOf(args[0]);
	}
	else {
	    maxNumberOfThreads = Integer.valueOf(args[0]);
	    minutesToRun = Double.parseDouble(args[1]);
	}

	System.out.println("using " + maxNumberOfThreads + " thread(s) and " +
			   "running for " + minutesToRun + " minute(s)...\n");

	// winning numbers
	Vector<Integer> winBalls = new Vector<Integer>(5);
	winBalls.addElement(8);
	winBalls.addElement(27);
	winBalls.addElement(34);
	winBalls.addElement(4);
	winBalls.addElement(19);
	int powerBall = 10;

	System.out.println("winning combination: " + winBalls + " " +
			   powerBall + "\n");

	long numberOfDraws = (long) 1.8e19;

	double millisToMins = 1./(1000.*60.);
	double epochStartMins = System.currentTimeMillis()*millisToMins;

	// start threads
	Lottery[] lotteries = new Lottery[maxNumberOfThreads];
	for(int i = 0; i < maxNumberOfThreads; i++) {
	    lotteries[i] = new Lottery(69, 26, maxNumberOfThreads);
	    lotteries[i].initializeGame(winBalls, powerBall, i);
	    lotteries[i].setIsTest(true);
	    lotteries[i].setNumberOfDraws(numberOfDraws);
	    lotteries[i].start();
	}

	double millisToRun = minutesToRun*1000;

	while(true) {
	    double epochNowMins = System.currentTimeMillis()*millisToMins;
	    if((epochNowMins-epochStartMins) >= minutesToRun) {

		// output
		Vector<Long> winningSeeds = new Vector<Long>();
		long numberOfCombinations = 0;
		for(int i = 0; i < maxNumberOfThreads; i++) {
		    winningSeeds.addAll(lotteries[i].getWinningSeeds());
		    numberOfCombinations +=
			lotteries[i].getNumberOfCombinations();
		}

		double chanceOfWinning = 100.*winningSeeds.size()/
		    numberOfCombinations;

		System.out.println("number of combinations  = " +
				   numberOfCombinations);
		if(winningSeeds.size() > 0) {
		    System.out.println("number of winning seeds = " +
				       winningSeeds.size());
		    System.out.println("chance of winning = " +
				       chanceOfWinning);
		}
		else {
		    System.out.println("no winning seeds");
		}

		System.exit(0);
	    }
	}




    }

}
