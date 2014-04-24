package algorithms;

import java.util.Collection;
import java.util.HashSet;
import java.util.Stack;

import algorithms.Node.NodeType;

public class DrawingEnumeration {

	private final VertexList V = new VertexList();
	private final EdgeList E = new EdgeList();

	private final StreamPrinter out = new SystemErrPrinter();

	public static int OUTPUT_LEVEL = 1;
	public static boolean KEEP_HISTORY = true;
	public static boolean KEEP_CIRCULAR_ORDERS = true;

	private IAlgorithmHooks hooks = null;

	class PathState {
		int crossings = 0;
		Edge e;
		HashSet<Edge> crossedEdges = new HashSet<Edge>();

		public PathState(Edge e) {
			this.e = e;
		}

		public PathState copy() {
			PathState nps = new PathState(e);
			nps.crossedEdges.addAll(crossedEdges);
			return nps;
		}

		public void print(StreamPrinter out) {
			out.println("Crossings: " + crossings);
		}
	}

	class DrawingState {
		final Stack<Vertex> unusedVertices;
		Stack<Edge> unusedEdges;
		
		History history;
		
		DrawingState()
		{
			unusedVertices = new Stack<Vertex>();
			unusedEdges = new Stack<Edge>();
		}
		
		DrawingState(DrawingState ds)
		{
			unusedEdges = (Stack<Edge>) ds.unusedEdges.clone();
			unusedVertices = (Stack<Vertex>) ds.unusedVertices.clone();
			history = (ds.history == null) ? null : new History(ds.history);
		}

		public void print(StreamPrinter out) {
			out.print("* " + unusedVertices.size() + " undrawn vertices: ");
			for (Vertex v : unusedVertices)
				out.print(v + " ");
			out.println("");
			out.print("* " + unusedEdges.size() + " undrawn edges: ");
			for (Edge e : unusedEdges)
				out.print(e + " ");
			out.println("");
		}

		public DrawingState copy() {
			return new DrawingState(this);
		}
	}

	public DrawingEnumeration(Collection<Vertex> V, Collection<Edge> E) {
		this.V.addAll(V);
		this.E.addAll(E);
	}

	public void setHooks(IAlgorithmHooks hooks) {
		this.hooks = hooks;
	}

	public void completeDrawing(VertexList initialRegion) throws Exception {

		if (initialRegion.size() != 4)
			throw new UnsupportedOperationException("Currently only initial regions of size four are accepted");

		PartialDrawing pd;
		DrawingState state;

		/* First use the non-crossing C4 as the initial region */
		pd = new PartialDrawing(V, E, initialRegion, PartialDrawing.InitialRegionConfiguration.NONCROSSING);
		state = new DrawingState();

		state.history = KEEP_HISTORY ? new History() : null; 
		state.unusedVertices.addAll(V);
		state.unusedVertices.removeAll(initialRegion);

		completeDrawing(pd, state);

		/* Next use the first type of crossing C4 as the initial region */
		pd = new PartialDrawing(V, E, initialRegion, PartialDrawing.InitialRegionConfiguration.CROSSING);
		state = new DrawingState();

		state.history = KEEP_HISTORY ? new History() : null; 
		state.unusedVertices.addAll(V);
		state.unusedVertices.removeAll(initialRegion);

		completeDrawing(pd, state);

		/* Finally use the second type of crossing C4 as the initial region */
		VertexList initialRegion2 = new VertexList();
		for (int i = 0; i < 4; i++)
			initialRegion2.add(initialRegion.get((i+1)%4));
		pd = new PartialDrawing(V, E, initialRegion2, PartialDrawing.InitialRegionConfiguration.CROSSING);
		state = new DrawingState();

		state.history = KEEP_HISTORY ? new History() : null; 
		state.unusedVertices.addAll(V);
		state.unusedVertices.removeAll(initialRegion);

		completeDrawing(pd, state);
	}

	private boolean completeDrawing(PartialDrawing pd, DrawingState ds) throws Exception {

		if (hooks.prune(pd))
			return true;
		pd.checkSanity();

		out.increaseIndent(2);

		try {
			// print some information
			if (OUTPUT_LEVEL >= 10) {
				out.println();
				out.println("completeDrawing called with the following input:");
				pd.print(out);
				ds.print(out);
			}

			// CASE 1: We run out of edges
			if (ds.unusedEdges.size() == 0) {
				if (ds.unusedVertices.size() == 0) {
					if (OUTPUT_LEVEL >= 2) {
						out.println("Constructed a drawing:");
						pd.print(out);
					}
					return hooks_drawingComplete(pd, ds);
				}

				DrawingState dsnew = ds.copy();
				Vertex newVertex = dsnew.unusedVertices.pop();

				for (int r = 0; r < pd.regionCount(); r++) {
					dsnew.unusedEdges.clear();
					PartialDrawing pdnew = new PartialDrawing(pd);
					Node newNode = pdnew.addFreeNode(newVertex, r);

					for (Edge e : E) {
						if (!e.isIncidentWith(newVertex))
							continue;
						if (pdnew.containsVertex(e.opposite(newVertex)))
							dsnew.unusedEdges.push(e);
					}

					if (OUTPUT_LEVEL >= 5)
						out.println("Added free node " + newNode + " to region " + pdnew.region(r));

					if (KEEP_HISTORY)
						dsnew.history.addFreeNode(newNode, pdnew.region(r).toString());
					
					boolean continueAlgorithm = completeDrawing(pdnew, dsnew);
					
					if (KEEP_HISTORY)
						dsnew.history.removeLast();
					
					if (!continueAlgorithm)
						return false;
				}
				return true;
			}

			// CASE 2: We have an edge to add
			DrawingState dsnew = ds.copy();
			Edge e = dsnew.unusedEdges.pop();

			Node nu = pd.findVertex(e.u);
			Node nv = pd.findVertex(e.v);

			assert (nu != null);
			assert (nv != null);

			if (!completePath(pd, dsnew, nu, nv, new PathState(e)))
				return false;

		} finally {
			out.increaseIndent(-2);
		}
		return true;
	}

	private boolean completePath(PartialDrawing pd, DrawingState ds, Node a, Node b, PathState ps) throws Exception {

//		if (hooks_prune(pd))
//			return true;
		pd.checkSanity();

		out.increaseIndent(2);

		// this function requires that at least one of a, b is on the boundary
		// of some region
		assert ((pd.getNodeType(a) == NodeType.Boundary) || (pd.getNodeType(b) == NodeType.Boundary));

		if (pd.getNodeType(b) == NodeType.Boundary) {
			Node t = a;
			a = b;
			b = t;
		}

		// so from now on, we may assume that a is a boundary node
		if (OUTPUT_LEVEL >= 10) {
			out.println();
			out.println("completePath called for nodes " + a + " and " + b);
			pd.print(out);
			ps.print(out);
		}

		int numberOfRecursions = 0;

		// 1) for each region that has both a and b on the boundary,
		// try adding a straight curve between a and b

		for (int r : pd.getCommonRegions(a, b)) {
			Region R = pd.region(r);
			if (OUTPUT_LEVEL >= 5)
				out.println("Adding chord between " + a + " and " + b + " in region " + R);

			PartialDrawing pdnew = pd.clone();
			pdnew.addChord(r, a, b, ps.e);

			if (KEEP_HISTORY)
				ds.history.addChord(a, b, R.toString());
			
			numberOfRecursions++;

			boolean continueAlgorithm = completeDrawing(pdnew, ds);
			if (KEEP_HISTORY)
				ds.history.removeLast();
			
			if (!continueAlgorithm)
				return false;
		}

		// 2) for each edge on the boundary of each region that b
		// is in make a crossing through that edge into
		// the `other side' and recurse

		for (Integer r: pd.getRegions(b)) {
			Region R = pd.region(r);
			for (Arc arc : R.getBoundaryArcs()) {
				Edge e = pd.getArcEdge(arc);
				if (e == ps.e)
					continue; // no self-intersecting curves
				if (e.sharesEndWith(ps.e)) // no two edges that share an end cross
					// each other
					continue;
				if (ps.crossedEdges.contains(e))
					continue; // no edge intersects another edge twice

				if (OUTPUT_LEVEL >= 5)
					out.println("Adding crossing from " + b + " through " + arc);

				PartialDrawing pdnew = pd.clone();
				PathState psnew = ps.copy();
				psnew.crossedEdges.add(e);
				psnew.crossings++;
				Node newNode = pdnew.addCrossEdge(r, b, arc, psnew.e);

				if (KEEP_HISTORY)
					ds.history.addCrossing(b, newNode, arc);

				boolean continueAlgorithm = completePath(pdnew, ds, a, newNode, psnew);
	
				if (KEEP_HISTORY)
					ds.history.removeLast();
			
				if (!continueAlgorithm)
					return false;
			}
		}

		out.increaseIndent(-2);

		return true;
	}

	private boolean hooks_prune(PartialDrawing pd) throws Exception {
		if (hooks == null)
			return false;
		return hooks.prune(pd);
	}

	private boolean hooks_drawingComplete(PartialDrawing pd, DrawingState ds) throws Exception {
		if (hooks == null)
			return true;
		return hooks.drawingComplete(pd, ds.history);
	}

	public void setSanityChecks(boolean checks) {
		PartialDrawing.SANITY_CHECKS = checks;
	}

	public void setOutputLevel(int outputLevel) {
		OUTPUT_LEVEL = outputLevel;
	}
}
