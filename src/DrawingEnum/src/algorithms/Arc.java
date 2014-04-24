package algorithms;

public class Arc implements Comparable<Arc> {

	// u and v are the endpoints of the arc
	// they are stored so that u < v
	public final Node u, v;

	public Arc(Node u, Node v) {

		if (u.compareTo(v) < 0) {
			this.u = u;
			this.v = v;
		} else {
			this.u = v;
			this.v = u;
		}
	}

	public boolean equals(Object o) {
		if (!(o instanceof Arc))
			return false;
		return equals((Arc) o);
	}

	public boolean equals(Arc a) {
		return (u == a.u) && (v == a.v);
	}

	public int hashCode() {
		return u.hashCode() + v.hashCode();
	}

	public String toString() {
		return "{" + u + "," + v + "}";
	}

	public int compareTo(Arc a) {
		int u_sign = u.compareTo(a.u);
		if (u_sign != 0)
			return u_sign;
		int v_sign = v.compareTo(a.v);
		return v_sign;
	}

	public boolean isIncidentWith(Node z) {
		return (u == z) || (v == z);
	}

	public Node opposite(Node z) {
		assert (isIncidentWith(z));
		if (z == u)
			return v;
		if (z == v)
			return u;
		return null;
	}

}
