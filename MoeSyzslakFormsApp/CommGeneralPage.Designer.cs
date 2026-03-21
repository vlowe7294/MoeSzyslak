
namespace MoeSyzslakFormsApp
{
    partial class CommGeneralPage
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
            this.BackupAudio = new System.Windows.Forms.CheckBox();
            this.ApplyButton = new System.Windows.Forms.Button();
            this.AuralCuesEnabled = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // BackupAudio
            // 
            this.BackupAudio.AutoSize = true;
            this.BackupAudio.Location = new System.Drawing.Point(4, 4);
            this.BackupAudio.Name = "BackupAudio";
            this.BackupAudio.Size = new System.Drawing.Size(100, 19);
            this.BackupAudio.TabIndex = 0;
            this.BackupAudio.Text = "Backup Audio";
            this.BackupAudio.UseVisualStyleBackColor = true;
            // 
            // ApplyButton
            // 
            this.ApplyButton.Location = new System.Drawing.Point(13, 124);
            this.ApplyButton.Name = "ApplyButton";
            this.ApplyButton.Size = new System.Drawing.Size(99, 23);
            this.ApplyButton.TabIndex = 1;
            this.ApplyButton.Text = "Apply";
            this.ApplyButton.UseVisualStyleBackColor = true;
            this.ApplyButton.Click += new System.EventHandler(this.OnApply);
            // 
            // AuralCuesEnabled
            // 
            this.AuralCuesEnabled.AutoSize = true;
            this.AuralCuesEnabled.Location = new System.Drawing.Point(4, 30);
            this.AuralCuesEnabled.Name = "AuralCuesEnabled";
            this.AuralCuesEnabled.Size = new System.Drawing.Size(128, 19);
            this.AuralCuesEnabled.TabIndex = 2;
            this.AuralCuesEnabled.Text = "Aural Cues Enabled";
            this.AuralCuesEnabled.UseVisualStyleBackColor = true;
            // 
            // CommGeneralPage
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.AuralCuesEnabled);
            this.Controls.Add(this.ApplyButton);
            this.Controls.Add(this.BackupAudio);
            this.Name = "CommGeneralPage";
            this.Size = new System.Drawing.Size(228, 150);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.CheckBox BackupAudio;
        private System.Windows.Forms.Button ApplyButton;
        private System.Windows.Forms.CheckBox AuralCuesEnabled;
    }
}
