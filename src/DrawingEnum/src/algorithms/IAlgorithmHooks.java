package algorithms;

public interface IAlgorithmHooks {

	public boolean drawingComplete(PartialDrawing pd, History history) throws Exception;

	public boolean prune(PartialDrawing pd) throws Exception;

}
