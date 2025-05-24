import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.awt.image.BufferedImage;

import javax.imageio.ImageIO;


// Load ảnh sprite và hiển thị animation
class SpriteAnimationPanel extends JPanel {
    private final BufferedImage[] frames;
    private int currentFrame = 0;
    private final Timer timer;

    /**
     * Tạo một panel hiển thị animation từ ảnh sprite.
     *
     * @param spritePath Đường dẫn đến ảnh sprite.
     * @param frameWidth Chiều rộng của mỗi frame.
     * @param frameHeight Chiều cao của mỗi frame.
     * @param frameCount Số lượng frame trong ảnh sprite.
     * @param delayMs Thời gian delay giữa các frame (ms).
     */
    public SpriteAnimationPanel(String spritePath, int frameWidth, int frameHeight, int frameCount, int delayMs) {
        setPreferredSize(new Dimension(frameWidth, frameHeight));
        frames = new BufferedImage[frameCount];

        try {
            BufferedImage spriteSheet = ImageIO.read(new File(spritePath));

            for (int i = 0; i < frameCount; i++) {
                frames[i] = spriteSheet.getSubimage(0, i * frameHeight, frameWidth, frameHeight);
            }

        } catch (Exception e) {
            throw new RuntimeException("Failed to load sprite: " + e.getMessage());
        }

        timer = new Timer(delayMs, e -> {
            currentFrame = (currentFrame + 1) % frames.length;
            repaint();
            if (currentFrame == 0) {
                stopAnimation(); // Dừng animation khi về frame đầu tiên
            }
        });
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        if (frames[currentFrame] != null) {
            g.drawImage(frames[currentFrame], 0, 0, this);
        }
    }

    public void startAnimation() {
        timer.start();
    }

    public void stopAnimation() {
        timer.stop();
    }

}

public class RPCGui2 {
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            JFrame frame = new JFrame("RPC Simulating");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setSize(700, 300); // mở rộng ngang để đủ chỗ cho ảnh
            frame.setLocationRelativeTo(null);
            frame.setLayout(new BorderLayout(10, 10));

            // ==== Form nhập liệu bên trái ====
            JPanel formPanel = new JPanel(new GridLayout(4, 2, 10, 10));
            formPanel.setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));

            JTextField num1Field = new JTextField();
            JTextField num2Field = new JTextField();
            JComboBox<String> opBox = new JComboBox<>(new String[]{"+", "-", "*", "/"});
            JLabel resultLabel = new JLabel("Result: ");
            JButton calcButton = new JButton("Execute");

            formPanel.add(new JLabel("First number:"));
            formPanel.add(num1Field);

            formPanel.add(new JLabel("Second number:"));
            formPanel.add(num2Field);

            formPanel.add(new JLabel("Operation:"));
            formPanel.add(opBox);

            formPanel.add(calcButton);
            formPanel.add(resultLabel);

            frame.add(formPanel, BorderLayout.CENTER);

            // ==== Ảnh bên phải (EAST) ====
            SpriteAnimationPanel animationPanel = new SpriteAnimationPanel(
                "images/sprite_for_rpc.png", 462, 497, 14, 1000 // path, width, height, frame count, delay (ms)
            );
            frame.add(animationPanel, BorderLayout.EAST);

            // ==== Xử lý nút Execute ====
            calcButton.addActionListener(e -> {
                String num1 = num1Field.getText().trim();
                String num2 = num2Field.getText().trim();
                String op = (String) opBox.getSelectedItem();
                
                String opCode = switch (op) {
                    case "+" -> "1";
                    case "-" -> "2";
                    case "*" -> "3";
                    case "/" -> "4";
                    default -> "0";
                };
                
                if (!num1.matches("\\d+") || !num2.matches("\\d+")) {
                    resultLabel.setText("Result: Please enter valid numbers.");
                    return;
                }
                
                try {
                    ProcessBuilder pb = new ProcessBuilder(".\\bin\\client.exe", opCode, num1, num2);
                    pb.redirectErrorStream(true);
                    Process process = pb.start();
                    
                    BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                    String output = reader.readLine();

                    if (output.equals("Error! Can't connect to server.")) {
                        // Thực hiện vẽ frame 14 -> frame 0
                    }
                    
                    animationPanel.startAnimation(); // Bắt đầu animation khi nhấn nút
                    // chờ animation kết thúc thì mới in ra kết quả

                    resultLabel.setText("Result: " + output);
                    process.waitFor();
                } catch (Exception ex) {
                    ex.printStackTrace();
                    resultLabel.setText("Error!");
                }
            });

            frame.setVisible(true);
        });
    }
}

// Panel ảnh co giãn chiều ngang/chiều dọc
// class ResizableImagePanel extends JPanel {
//     private final Image image;

//     public ResizableImagePanel(String path) {
//         ImageIcon icon = new ImageIcon(path);
//         image = icon.getImage();
//     }

//     @Override
//     protected void paintComponent(Graphics g) {
//         super.paintComponent(g);
//         if (image != null) {
//             int panelWidth = getWidth();
//             int panelHeight = getHeight();

//             // Tính toán giữ tỷ lệ ảnh gốc
//             float imgRatio = (float) image.getWidth(this) / image.getHeight(this);
//             float panelRatio = (float) panelWidth / panelHeight;

//             int drawWidth, drawHeight;

//             if (panelRatio > imgRatio) {
//                 drawHeight = panelHeight;
//                 drawWidth = (int) (drawHeight * imgRatio);
//             } else {
//                 drawWidth = panelWidth;
//                 drawHeight = (int) (drawWidth / imgRatio);
//             }

//             int x = (panelWidth - drawWidth) / 2;
//             int y = (panelHeight - drawHeight) / 2;

//             g.drawImage(image, x, y, drawWidth, drawHeight, this);
//         }
//     }
// }
