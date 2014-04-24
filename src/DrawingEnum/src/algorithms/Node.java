package algorithms;

public class Node implements Comparable<Node> {

	public enum NodeType {
		Unknown, Boundary, Leaf, Free
	};

	public final Vertex vertex;
	public final int numericalLabel;
	private final int serialNumber; 

	public static int nodeCount = 0;

	public Node(int numericalLabel) {
		this.vertex = null;
		this.numericalLabel = numericalLabel;
		serialNumber = nodeCount++;
	}

	public Node(Vertex vertex) {
		this.vertex = vertex;
		numericalLabel = 0;

		serialNumber = nodeCount++;
	}

	public String toString() {
		if (vertex == null)
			return "s" + numericalLabel;
		else
			return vertex.toString();
	}

	public int compareTo(Node n) {
		return n.serialNumber - serialNumber;
	}
}
