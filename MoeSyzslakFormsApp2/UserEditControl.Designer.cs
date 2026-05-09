namespace MoeSyzslakFormsApp2
{
    partial class UserEditControl
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
            UserNameValue = new Label();
            EmailLabel = new Label();
            EmailTextBox = new TextBox();
            SuspendLayout();
            // 
            // UserNameLabel
            // 
            UserNameLabel.AutoSize = true;
            UserNameLabel.Location = new Point(14, 30);
            UserNameLabel.Name = "UserNameLabel";
            UserNameLabel.Size = new Size(65, 15);
            UserNameLabel.TabIndex = 0;
            UserNameLabel.Text = "User Name";
            // 
            // UserNameValue
            // 
            UserNameValue.AutoSize = true;
            UserNameValue.Location = new Point(119, 30);
            UserNameValue.Name = "UserNameValue";
            UserNameValue.Size = new Size(0, 15);
            UserNameValue.TabIndex = 1;
            // 
            // EmailLabel
            // 
            EmailLabel.AutoSize = true;
            EmailLabel.Location = new Point(17, 79);
            EmailLabel.Name = "EmailLabel";
            EmailLabel.Size = new Size(36, 15);
            EmailLabel.TabIndex = 2;
            EmailLabel.Text = "Email";
            // 
            // EmailTextBox
            // 
            EmailTextBox.Location = new Point(142, 78);
            EmailTextBox.Name = "EmailTextBox";
            EmailTextBox.Size = new Size(176, 23);
            EmailTextBox.TabIndex = 3;
            // 
            // UserEditControl
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            Controls.Add(EmailTextBox);
            Controls.Add(EmailLabel);
            Controls.Add(UserNameValue);
            Controls.Add(UserNameLabel);
            Name = "UserEditControl";
            Size = new Size(353, 211);
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Label UserNameLabel;
        private Label UserNameValue;
        private Label EmailLabel;
        private TextBox EmailTextBox;
    }
}
