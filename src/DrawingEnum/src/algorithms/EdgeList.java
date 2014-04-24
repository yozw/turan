package algorithms;

import java.util.ArrayList;

public class EdgeList extends ArrayList<Edge> {

	private static final long serialVersionUID = 1L;

	public Edge getEdge(Vertex u, Vertex v) {
		for (Edge e : this)
			if (e.hasEndpoints(u, v))
				return e;
		return null;
	}
}
