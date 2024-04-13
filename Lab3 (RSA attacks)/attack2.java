import java.math.BigInteger;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class attack2 {

    public static void main(String[] args) throws Exception {
        List<String> lines = Files.readAllLines(Path.of("MitM_RSA_2048_20_regular_09.txt"));
        if (lines.size() != 2) {
            throw new RuntimeException();
        }
        BigInteger C = parse(lines.get(0));
        BigInteger N = parse(lines.get(1));
        int e = 65537;
        int l = 20;


        long start = System.currentTimeMillis();
        Map<BigInteger, BigInteger> tToTe = new HashMap<>();
        Map<BigInteger, BigInteger> teToT = new HashMap<>();

        BigInteger E = big(e);
        for (int t = 1; t <= (1 << (l / 2)); t++) {
            var T = big(t);
            var Te = T.modPow(E, N);
            tToTe.put(T, Te);
            teToT.put(Te, T);
        }

        Pair pair = findTs(C, N, l, tToTe, teToT);
        System.out.println("elapsed: " + (System.currentTimeMillis() - start) + " ms");

        if (pair == null) {
            System.out.println("no result");
            return;
        }

        System.out.println("T = " + pair.T);
        System.out.println("S = " + pair.S);
        var M = pair.T.multiply(pair.S);
        System.out.println("M = " + M);

        var X = pair.T.multiply(pair.S).modPow(E, N);
        System.out.println("verify: " + X.equals(C));
    }

    static Pair findTs(BigInteger C, BigInteger N, int l, Map<BigInteger, BigInteger> tToTe, Map<BigInteger, BigInteger> teToT) {
        for (int s = 1; s <= (1 << (l / 2)); s++) {
            var S = big(s);
            var Ms = C.multiply(tToTe.get(S).modInverse(N)).mod(N);
            var T = teToT.get(Ms);
            if (T != null) {
                return new Pair(T, S);
            }
        }
        return null;
    }

    static BigInteger parse(String str) {
        return new BigInteger(str.substring(6), 16);
    }

    static BigInteger big(long val) {
        return BigInteger.valueOf(val);
    }

    static class Pair {

        final BigInteger T;
        final BigInteger S;

        Pair(BigInteger T, BigInteger S) {
            this.T = T;
            this.S = S;
        }

    }

}
