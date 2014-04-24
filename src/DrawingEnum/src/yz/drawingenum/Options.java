package yz.drawingenum;

public class Options {
  public boolean generateDotFiles = false;
  public boolean compileDotFiles = false;

  public boolean calculateMinCrossings = false;
  public boolean generateOptimalDotFile = false;
  public boolean compileOptimalDotFile = false;
  public boolean outputAsVector = false;

  public boolean keepCircularOrders = false;
  public boolean keepHistory = false;

  public int outputLevel = 1;
  public boolean sanityChecks = false;

  public int crossingLimit = Integer.MAX_VALUE;
  public int crossingTarget = -1;
}
