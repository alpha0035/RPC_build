import javax.swing.*;
import java.awt.*;
import java.io.*;

public class RPCGui {
    public static void main(String[] args) {
        JFrame frame = new JFrame("RPC simulating");
        frame.setSize(400, 250);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setLocationRelativeTo(null);

        JTextField num1Field = new JTextField(10);
        JTextField num2Field = new JTextField(10);
        JComboBox<String> opBox = new JComboBox<>(new String[]{"+", "-", "*", "/"});
        JButton calcButton = new JButton("Excute");
        JLabel resultLabel = new JLabel("Result: ");

        JPanel panel = new JPanel(new GridLayout(5, 2, 10, 10));
        panel.setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));

        panel.add(new JLabel("First number:"));
        panel.add(num1Field);

        panel.add(new JLabel("Second number:"));
        panel.add(num2Field);

        panel.add(new JLabel("Operation:"));
        panel.add(opBox);

        panel.add(calcButton);
        panel.add(resultLabel);

        frame.add(panel);
        frame.setVisible(true);

        calcButton.addActionListener(e -> {
            String num1 = num1Field.getText().trim();
            String num2 = num2Field.getText().trim();
            String op = (String) opBox.getSelectedItem();
            if (op.equals("+")) {
                op = "1";
            } else if (op.equals("-")) {
                op = "2";
            } else if (op.equals("*")) {
                op = "3";
            } else if (op.equals("/")) {
                op = "4";
            }

            if (!num1.matches("\\d+") || !num2.matches("\\d+")) {
                resultLabel.setText("Please enter valid numbers.");
                return;
            }

            try {
                ProcessBuilder pb = new ProcessBuilder(".\\bin\\client.exe", op, num1, num2);
                pb.redirectErrorStream(true);
                Process process = pb.start();

                BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                String output = reader.readLine();

                resultLabel.setText("Result: " + output);
                process.waitFor();
            } catch (Exception ex) {
                ex.printStackTrace();
                resultLabel.setText("Error!");
            }
        });
    }
}
