# Commit and change log format

- Last updated 2026-05-25

---

## Commit format

```
<type>[optional scope]: <summary>

[optional body]

[optional BREAKING-CHANGE]
```

Example:
```
docs: Update changelog and commit format

This addresses #405.
```

Commits after Dn0.5.3 must adhere to [Conventional Commits v1.0.0](https://www.conventionalcommits.org/en/v1.0.0/#specification).

A pull request may have multiple commits of varying category types, just be sure that each commit is atomic in category, and to update the change log accordingly.

## Commit category types and scopes


- **Category types** (any other category types are allowed, but here are the main ones)
	- `feat`
		- addition of new features or functionality
	- `fix`
		- fixes to features functionality to work as intended
	- `update`
		- distinct from `feat` in that it does not add new features, but updates it.
	- `docs`
		- updates to documentation, including licenses and license year text, icons and logos
	- `ci`
		- continuous integration, such as GitHub Actions
	- `build`
		- build scripts, VS solution and project files
	- `refactor`
		- change or improve source code without changing external behavior
	- `revert`
		- undo any previous change
	- `wip`
		- work-in-progress commit: must not end up in the final pushed commit history!
- **Category scopes** (i.e. this commit mainly concerns with:)
	- `nsfdrv`
		-  the 6502 asm sound engine used in ROM / assembly export and its supporting infrastructure in the tracker.
	- `track-play`
		- the player function in the tracker, including pattern playback, core emulators, and audio output
	- `track-ui`
		- the tracker's interface (graphical or not), including the import/export of modules, and the interfaces that modify module data
	- `repo`
		- repository changes, for example, READMEs, directory renames or restructuring.
	- `github`
		- GitHub repository-related files, such as templates (example: [`pull_request_template.md`](Dn-FamiTracker/.github/pull_request_template.md)), text, documentation, etc.

## Change log format

- `<summary> ([issue_author] [commit_author] [#<issue number>] [#<PR number>])`
- only put in issue author and number if it exists
- only put one author if issue and commit author is the same
- sort by category, then by issue number, then by PR number

Example:

- Fix access violation in MRU submenu list update (@eugene-s-nesdev @Gumball2415 #243 #214)
- Pick song length mode when typing length time in WAV export dialog (@nyanpasu64 #346)

## Change log categories

- **Breaking changes**
	- Changes that has any non-compatibility with the previous version.
- **Important changes**
	- Summary of immediate and obvious changes to the program. This is to be done by the repository maintainer.
- **Improvements**
	- Additions and refactoring.
	- This corresponds to commit categories `feat` and `update`
		- with scopes `nsfdrv`, `track-play`, and `track-ui`.
- **Bug fixes**
	- Any bug fixes.
	- This corresponds to commit category `fix`
		- with scopes `nsfdrv`, `track-play`, and `track-ui`.
- **Internal**
	- Any meta changes, including changes regarding to the repository or CI
	- This corresponds to commit scopes `github`, `repo`, `ci`, and `build`.
	- this also corresponds to commit categories `docs`

If there are no changes of a particular category, it must be omitted.
