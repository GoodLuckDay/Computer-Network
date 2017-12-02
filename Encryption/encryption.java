import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class encryption {
    static final int ALPABETS = ('z'-'a'+1);
    public static void main(String[] args) throws IOException {
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(System.in));
        String inputSttring = bufferedReader.readLine();

        inputSttring = simpleEncryption(inputSttring);
        inputSttring = simpleDecryption(inputSttring);
        inputSttring = mediumEncryption(inputSttring);
        inputSttring = mediumDecryption(inputSttring);
        inputSttring = ultraEncryption(inputSttring);
        inputSttring = ultraDecryption(inputSttring);
    }

    private static String ultraDecryption(String inputSttring) {
        String result = "";
        int index = 0;
        for(char chr : inputSttring.toCharArray()){
            chr = (char)(chr - Math.pow(index++,2));
            result += isLower(chr) || isUpper(chr) ? (char)chr : (char)(chr + ALPABETS);
        }
        System.out.println("Ultra Encryption : "+result);
        return result;
    }

    private static String mediumDecryption(String inputSttring) {
        String result = "";
        int index = 0;
        for(char chr : inputSttring.toCharArray()){
            chr = (char)(chr - index++);
            result += isLower(chr) || isUpper(chr) ? (char)chr : (char)(chr + ALPABETS);
        }
        System.out.println("Medium Decryption : "+result);
        return result;
    }

    private static String simpleDecryption(String inputSttring) {
        String result = "";
        for(char chr : inputSttring.toCharArray()){
            chr = (char)(chr - 1);
            result += isLower(chr) || isUpper(chr) ? (char)chr : (char)(chr + ALPABETS);
        }
        System.out.println("Simple Decryption : "+result);
        return result;
    }

    private static String ultraEncryption(String inputSttring) {
        String result = "";
        int index = 0;
        for(char chr : inputSttring.toCharArray()){
            chr = (char)(chr + Math.pow(index++,2));
            result += isLower(chr) || isUpper(chr) ? (char)chr : (char)(chr - ALPABETS);
        }
        System.out.println("Ultra Encryption : "+result);
        return result;
    }

    private static String mediumEncryption(String inputSttring) {
        String result = "";
        int index = 0;
        for(char chr : inputSttring.toCharArray()){
            chr = (char)(chr + index++);
            result += isLower(chr) || isUpper(chr) ? (char)chr : (char)(chr - ALPABETS);
        }
        System.out.println("Medium Encryption : "+result);
        return result;
    }

    private static String simpleEncryption(String inputSttring) {
        String result = "";
        for(char chr : inputSttring.toCharArray()){
            chr = (char)(chr + 1);
            result += isLower(chr) || isUpper(chr) ? (char)chr : (char)(chr - ALPABETS);
        }
        System.out.println("Simple Encryption : "+result);
        return result;
    }

    private static boolean isLower(char chr){
        return ('a' <= chr && chr <= 'z');
    }

    private static boolean isUpper(char chr){
        return ('A' <= chr && chr <= 'Z');
    }
}
