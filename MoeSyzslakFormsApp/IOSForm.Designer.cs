
namespace MoeSyzslakFormsApp
{
    partial class IOSForm
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.theCommGeneralPage = new MoeSyzslakFormsApp.CommGeneralPage();
            this.SuspendLayout();
            // 
            // theCommGeneralPage
            // 
            this.theCommGeneralPage.Location = new System.Drawing.Point(538, 21);
            this.theCommGeneralPage.Name = "theCommGeneralPage";
            this.theCommGeneralPage.Size = new System.Drawing.Size(237, 146);
            this.theCommGeneralPage.TabIndex = 0;
            // 
            // IOSForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.theCommGeneralPage);
            this.Name = "IOSForm";
            this.Text = "IOSForm";
            this.ResumeLayout(false);

        }

        #endregion

        private CommGeneralPage theCommGeneralPage;
    }
}