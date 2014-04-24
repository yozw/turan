package algorithms;

public class Vertex implements Comparable<Vertex> {

	public final String name;

	private int index = 0;

	public Vertex(String name) {
		this.name = name;
	}

	public String toString() {
		return this.name;
	}

	@Override
	public int compareTo(Vertex v) {
		return v.index - index;
	}
}
