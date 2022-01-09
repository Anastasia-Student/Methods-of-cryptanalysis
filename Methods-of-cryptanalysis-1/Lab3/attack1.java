import java.math.BigInteger;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

public class attack1 {

    public static void main(String[] args) throws Exception {
        List<Pair> system = readSystem(Path.of("SE_RSA_1024_5_hard_09.txt"));
        int e = 5;

        long start = System.currentTimeMillis();
        BigInteger C = crt(system);
        BigInteger M = nthRoot(C, e);
        System.out.println("elapsed: " + (System.currentTimeMillis() - start) + " ms");

        System.out.println("C = " + C.toString(16));
        System.out.println("M = " + M.toString(16));
        System.out.println("verify: " + verify(system, e, C, M));
    }

    static List<Pair> readSystem(Path path) throws Exception {
        List<String> lines = Files.readAllLines(path);
        if (lines.isEmpty() || lines.size() % 2 != 0) {
            throw new RuntimeException();
        }
        List<Pair> ret = new ArrayList<>(lines.size() / 2);
        for (int i = 0; i < lines.size(); i += 2) {
            ret.add(new Pair(parse(lines.get(i)), parse(lines.get(i + 1))));
        }
        return ret;
    }

    static BigInteger parse(String str) {
        return new BigInteger(str.substring(7), 16);
    }

    static BigInteger crt(List<Pair> system) {
        var N = BigInteger.ONE;
        for (Pair pair : system) {
            N = N.multiply(pair.N);
        }
        var X = BigInteger.ZERO;
        for (Pair pair : system) {
            var Ni = N.divide(pair.N);
            X = X.add(pair.C.multiply(Ni).multiply(Ni.modInverse(pair.N)));
        }
        return X.mod(N);
    }

    static BigInteger nthRoot(BigInteger X, int n) {
        var n1 = n - 1;
        var s = X.add(BigInteger.ONE);
        var u = X;
        while (u.compareTo(s) < 0) {
            s = u;
            u = u.multiply(big(n1)).add(X.divide(u.pow(n1))).divide(big(n));
        }
        return s;
    }

    static boolean verify(List<Pair> system, int e, BigInteger C, BigInteger M) {
        var N = BigInteger.ONE;
        for (Pair pair : system) {
            N = N.multiply(pair.N);
        }
        return M.modPow(big(e), N).equals(C);
    }

    static BigInteger big(long val) {
        return BigInteger.valueOf(val);
    }

    static class Pair {

        final BigInteger C;
        final BigInteger N;

        Pair(BigInteger C, BigInteger N) {
            this.C = C;
            this.N = N;
        }

    }

}
