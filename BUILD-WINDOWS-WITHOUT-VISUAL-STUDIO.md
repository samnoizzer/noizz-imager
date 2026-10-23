# Build a Windows VST3 without installing Visual Studio on your PC

This project includes a GitHub Actions workflow that builds the plugin on a hosted Windows runner. You do not need Visual Studio or CMake installed locally. You do need a GitHub account and to upload this project to a repository you own.

## Steps

1. Sign in to GitHub and create a **new repository**. For a private project, choose **Private**.
2. Upload the contents of this `noizz_imager` folder to the repository (the `.github/workflows/build-windows-vst3.yml` file must be included at the repository root).
3. Open the repository's **Actions** tab and enable Actions if prompted.
4. Select **Build NOIZZ IMAGER Windows VST3** in the workflow list.
5. Click **Run workflow** and confirm **Run workflow**.
6. Wait for the run to finish. If it succeeds, open that run and download the artifact named `NOIZZ-IMAGER-Windows-x64-VST3`.
7. Extract the downloaded ZIP. Copy the included `.vst3` bundle to `C:\Program Files\Common Files\VST3`, then restart/rescan plugins in your DAW.

## Important

- The workflow is a build path, not proof that the plugin has already compiled. Compilation must succeed in GitHub Actions before an artifact is available.
- The build downloads JUCE from its official Git repository, so the hosted runner needs internet access.
- If the build fails, open the workflow run and inspect the failed step's log. Share the error log so the project can be corrected.
- This is a prototype and still needs audio/phase, host compatibility, and preset workflow testing before production use.
