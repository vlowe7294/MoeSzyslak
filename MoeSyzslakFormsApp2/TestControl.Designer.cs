namespace MoeSyzslakFormsApp2
{
    partial class TestControl
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
            ClassListBox = new ListBox();
            TestButton = new Button();
            ResultsListView = new ListView();
            TimeColumnHeader = new ColumnHeader();
            TextColumnHeader = new ColumnHeader();
            ResultLabel = new Label();
            SuspendLayout();
            // 
            // ClassListBox
            // 
            ClassListBox.FormattingEnabled = true;
            ClassListBox.Items.AddRange(new object[] { "Test Self", "User" });
            ClassListBox.Location = new Point(18, 13);
            ClassListBox.Name = "ClassListBox";
            ClassListBox.Size = new Size(271, 34);
            ClassListBox.TabIndex = 0;
            // 
            // TestButton
            // 
            TestButton.Location = new Point(24, 73);
            TestButton.Name = "TestButton";
            TestButton.Size = new Size(126, 29);
            TestButton.TabIndex = 1;
            TestButton.Text = "Run Test";
            TestButton.UseVisualStyleBackColor = true;
            TestButton.Click += OnRunTest;
            // 
            // ResultsListView
            // 
            ResultsListView.Columns.AddRange(new ColumnHeader[] { TimeColumnHeader, TextColumnHeader });
            ResultsListView.Location = new Point(36, 155);
            ResultsListView.Name = "ResultsListView";
            ResultsListView.Size = new Size(823, 220);
            ResultsListView.TabIndex = 2;
            ResultsListView.UseCompatibleStateImageBehavior = false;
            ResultsListView.View = View.Details;
            // 
            // TimeColumnHeader
            // 
            TimeColumnHeader.Text = "Time (ms)";
            TimeColumnHeader.Width = 100;
            // 
            // TextColumnHeader
            // 
            TextColumnHeader.Text = "Message";
            TextColumnHeader.Width = 400;
            // 
            // ResultLabel
            // 
            ResultLabel.AutoSize = true;
            ResultLabel.ForeColor = Color.ForestGreen;
            ResultLabel.Location = new Point(47, 409);
            ResultLabel.Name = "ResultLabel";
            ResultLabel.Size = new Size(81, 15);
            ResultLabel.TabIndex = 3;
            ResultLabel.Text = "Ready for Test";
            // 
            // TestControl
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            Controls.Add(ResultLabel);
            Controls.Add(ResultsListView);
            Controls.Add(TestButton);
            Controls.Add(ClassListBox);
            Name = "TestControl";
            Size = new Size(928, 472);
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private ListBox ClassListBox;
        private Button TestButton;
        private ListView ResultsListView;
        private ColumnHeader TimeColumnHeader;
        private ColumnHeader TextColumnHeader;
        private Label ResultLabel;
    }
}
