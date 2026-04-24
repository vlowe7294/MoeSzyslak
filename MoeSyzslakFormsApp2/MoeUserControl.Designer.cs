namespace MoeSyzslakFormsApp2
{
    partial class MoeUserControl
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            UserNameLabel = new Label();
            NameTextBox = new TextBox();
            LoginButton = new Button();
            PasswordTextBox = new TextBox();
            PasswordLabel = new Label();
            SuspendLayout();
            // 
            // UserNameLabel
            // 
            UserNameLabel.AutoSize = true;
            UserNameLabel.Location = new Point(13, 22);
            UserNameLabel.Name = "UserNameLabel";
            UserNameLabel.Size = new Size(39, 15);
            UserNameLabel.TabIndex = 0;
            UserNameLabel.Text = "Name";
            // 
            // NameTextBox
            // 
            NameTextBox.Location = new Point(94, 19);
            NameTextBox.Name = "NameTextBox";
            NameTextBox.Size = new Size(194, 23);
            NameTextBox.TabIndex = 0;
            // 
            // LoginButton
            // 
            LoginButton.Location = new Point(23, 145);
            LoginButton.Name = "LoginButton";
            LoginButton.Size = new Size(133, 23);
            LoginButton.TabIndex = 2;
            LoginButton.Text = "Login";
            LoginButton.UseVisualStyleBackColor = true;
            LoginButton.Click += OnLogin;
            // 
            // PasswordTextBox
            // 
            PasswordTextBox.Location = new Point(94, 68);
            PasswordTextBox.Name = "PasswordTextBox";
            PasswordTextBox.PasswordChar = '*';
            PasswordTextBox.Size = new Size(194, 23);
            PasswordTextBox.TabIndex = 1;
            // 
            // PasswordLabel
            // 
            PasswordLabel.AutoSize = true;
            PasswordLabel.Location = new Point(13, 71);
            PasswordLabel.Name = "PasswordLabel";
            PasswordLabel.Size = new Size(57, 15);
            PasswordLabel.TabIndex = 3;
            PasswordLabel.Text = "Password";
            // 
            // MoeUserControl
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            Controls.Add(PasswordTextBox);
            Controls.Add(PasswordLabel);
            Controls.Add(LoginButton);
            Controls.Add(NameTextBox);
            Controls.Add(UserNameLabel);
            Name = "MoeUserControl";
            Size = new Size(355, 186);
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Label UserNameLabel;
        private TextBox NameTextBox;
        private Button LoginButton;
        private TextBox PasswordTextBox;
        private Label PasswordLabel;
    }
}
