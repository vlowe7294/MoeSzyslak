
namespace MoeSyzslakFormsApp
{
    partial class MainForm
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.TestButton = new System.Windows.Forms.Button();
            this.MainPropertyGrid = new System.Windows.Forms.PropertyGrid();
            this.SuspendLayout();
            // 
            // TestButton
            // 
            this.TestButton.Location = new System.Drawing.Point(28, 45);
            this.TestButton.Name = "TestButton";
            this.TestButton.Size = new System.Drawing.Size(149, 41);
            this.TestButton.TabIndex = 0;
            this.TestButton.Text = "Diagnostic Window";
            this.TestButton.UseVisualStyleBackColor = true;
            this.TestButton.Click += new System.EventHandler(this.OnTestButton);
            // 
            // MainPropertyGrid
            // 
            this.MainPropertyGrid.Location = new System.Drawing.Point(513, 45);
            this.MainPropertyGrid.Name = "MainPropertyGrid";
            this.MainPropertyGrid.Size = new System.Drawing.Size(258, 366);
            this.MainPropertyGrid.TabIndex = 1;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.MainPropertyGrid);
            this.Controls.Add(this.TestButton);
            this.Name = "MainForm";
            this.Text = "Moe Syszlak";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button TestButton;
        private System.Windows.Forms.PropertyGrid MainPropertyGrid;
    }
}

