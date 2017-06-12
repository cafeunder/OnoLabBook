require 'base64'

module ReVIEW
	class Compiler
		defblock :nest, 0
	end

	class Builder
		def nest(lines)
			nestsub(lines, 'none')
		end

		protected
		def nestsub(lines, type)
			env = nil
			puts '\begin{enumerate}' if type === 'latex'
			puts '<ol>' if type === 'html'
			lines.each do | line |
				m = line.match(/^(\s*)(\d+\.|\*+|\-+|\++)\s+(.*)$/)
				next if m.nil?

				if /^(\*+|\-+|\++)$/ === m[2]
					if type === 'latex'
						puts '\begin{itemize}' if env.nil?
						env = 'itemize'
					elsif type === 'html'
						puts '<ul>' if env.nil?
						env = 'ul'
					end
				elsif m[1].length > 1
					if type === 'latex'
						puts '\begin{enumerate}' if env.nil?
						env = 'enumerate'
					elsif type === 'html'
						puts '<ol>' if env.nil?
						env = 'ol'
					end
				else
					if type === 'latex'
						puts '\end{' + env + '}' unless env.nil?
					elsif type === 'html'
						puts '</' + env + '>' unless env.nil?
					end
					env = nil
				end

				if type === 'latex'
					puts '\item ' + m[3]
				elsif type === 'html'
					puts '<li>' + m[3] + '</li>'
				else
					puts m[3]
				end
			end

			puts '\end{enumerate}' if type === 'latex'
			puts '</ol>' if type === 'html'
		end
	end

	class LATEXBuilder
		def nest(lines) nestsub(lines, 'latex') end
	end

	class HTMLBuilder
		def nest(lines) nestsub(lines, 'html') end
	end
end
