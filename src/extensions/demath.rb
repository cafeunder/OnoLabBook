require 'open3'
require 'base64'

module ReVIEW
	class Compiler
		defblock :demathraw, 0
	end

	class Builder
		def demathraw(lines)
			texequation(Base64.decode64(lines.join('')).split('\n'))
		end
	end

	class LATEXBuilder
		def demathraw(lines)
			puts '\vspace*{-\baselineskip}\begin{align*}'
			puts Base64.decode64(lines.join(''))
			puts '\end{align*}'
		end
	end

	class HTMLBuilder
		def demathraw(lines)
			stdout, stderr, status = Open3.capture3(
				"tex2png.rb",
				:stdin_data => Base64.decode64(lines.join(''))
			)

			data = stdout.delete("\n")
			puts '<div class="image">'
			puts "<img src=\"data:image/png;base64,#{data}\" class=\"width-090per\" />"
			puts '</div>'
		end
	end
end
