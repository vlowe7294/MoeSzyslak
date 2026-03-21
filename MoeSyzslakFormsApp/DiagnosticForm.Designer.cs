
namespace MoeSyzslakFormsApp
{
    partial class DiagnosticForm
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
            if (m_hObj > 0)
            {
                MoeSzyslakLibrary.DestroyMoeSzyslakHandle(m_hObj);
                m_hObj = 0;
            }

            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.VersionLabel = new System.Windows.Forms.Label();
            this.ErrorLabel = new System.Windows.Forms.Label();
            this.UnitTestButton = new System.Windows.Forms.Button();
            this.OutputTextBox = new System.Windows.Forms.RichTextBox();
            this.MemoryCheckBox = new System.Windows.Forms.CheckBox();
            this.TestObjectListBox = new System.Windows.Forms.ListBox();
            this.SuspendLayout();
            // 
            // VersionLabel
            // 
            this.VersionLabel.AutoSize = true;
            this.VersionLabel.Location = new System.Drawing.Point(64, 31);
            this.VersionLabel.Name = "VersionLabel";
            this.VersionLabel.Size = new System.Drawing.Size(45, 15);
            this.VersionLabel.TabIndex = 0;
            this.VersionLabel.Text = "Version";
            // 
            // ErrorLabel
            // 
            this.ErrorLabel.AutoSize = true;
            this.ErrorLabel.ForeColor = System.Drawing.Color.Red;
            this.ErrorLabel.Location = new System.Drawing.Point(64, 76);
            this.ErrorLabel.Name = "ErrorLabel";
            this.ErrorLabel.Size = new System.Drawing.Size(0, 15);
            this.ErrorLabel.TabIndex = 1;
            // 
            // UnitTestButton
            // 
            this.UnitTestButton.Location = new System.Drawing.Point(186, 64);
            this.UnitTestButton.Name = "UnitTestButton";
            this.UnitTestButton.Size = new System.Drawing.Size(136, 31);
            this.UnitTestButton.TabIndex = 2;
            this.UnitTestButton.Text = "Unit Test";
            this.UnitTestButton.UseVisualStyleBackColor = true;
            this.UnitTestButton.Click += new System.EventHandler(this.OnUnitTest);
            // 
            // OutputTextBox
            // 
            this.OutputTextBox.Location = new System.Drawing.Point(64, 206);
            this.OutputTextBox.Name = "OutputTextBox";
            this.OutputTextBox.Size = new System.Drawing.Size(538, 96);
            this.OutputTextBox.TabIndex = 3;
            this.OutputTextBox.Text = "";
            // 
            // MemoryCheckBox
            // 
            this.MemoryCheckBox.AutoSize = true;
            this.MemoryCheckBox.Location = new System.Drawing.Point(386, 31);
            this.MemoryCheckBox.Name = "MemoryCheckBox";
            this.MemoryCheckBox.Size = new System.Drawing.Size(107, 19);
            this.MemoryCheckBox.TabIndex = 4;
            this.MemoryCheckBox.Text = "Check Memory";
            this.MemoryCheckBox.UseVisualStyleBackColor = true;
            // 
            // TestObjectListBox
            // 
            this.TestObjectListBox.FormattingEnabled = true;
            this.TestObjectListBox.ItemHeight = 15;
            this.TestObjectListBox.Items.AddRange(new object[] {
            "Testing"});
            this.TestObjectListBox.Location = new System.Drawing.Point(71, 64);
            this.TestObjectListBox.Name = "TestObjectListBox";
            this.TestObjectListBox.Size = new System.Drawing.Size(88, 79);
            this.TestObjectListBox.TabIndex = 5;
            // 
            // DiagnosticForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.TestObjectListBox);
            this.Controls.Add(this.MemoryCheckBox);
            this.Controls.Add(this.OutputTextBox);
            this.Controls.Add(this.UnitTestButton);
            this.Controls.Add(this.ErrorLabel);
            this.Controls.Add(this.VersionLabel);
            this.Name = "DiagnosticForm";
            this.Text = "Diagnostics";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label VersionLabel;
        private System.Windows.Forms.Label ErrorLabel;
        private System.Windows.Forms.Button UnitTestButton;
        private System.Windows.Forms.RichTextBox OutputTextBox;
        private System.Windows.Forms.CheckBox MemoryCheckBox;
        private System.Windows.Forms.ListBox TestObjectListBox;
    }
}