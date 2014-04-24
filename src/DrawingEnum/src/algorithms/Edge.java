package algorithms;

public class Edge {

	public final Vertex u, v;

	public Edge(Vertex u, Vertex v) {
		this.u = u;
		this.v = v;
	}

	public boolean isIncidentWith(Vertex z) {
		return (u == z) || (v == z);
	}

	public Vertex opposite(Vertex z) {
		assert (isIncidentWith(z));

		if (u == z)
			return v;
		if (v == z)
			return u;

		return null;
	}

	public String toString() {
		return "{" + u + "," + v + "}";
	}

	public boolean hasEndpoints(Vertex y, Vertex z) {
		return isIncidentWith(y) && isIncidentWith(z);
	}

	public boolean sharesEndWith(Edge e) {
		return isIncidentWith(e.u) || isIncidentWith(e.v);
	}
}
