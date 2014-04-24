package yz.drawingenum.algorithms;

public interface Callback {
  public boolean drawingComplete(PartialDrawing pd, History history) throws Exception;
  public boolean prune(PartialDrawing pd) throws Exception;
}
